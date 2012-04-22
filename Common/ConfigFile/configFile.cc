#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

#include "./H/configFile.h"

/**
 * Config file constructor.
 *
 * Creates a mutex for protecting the STL map container. Reads
 * configuration from specified file.
 *
 *
 * @param   name    Name of the file that contains configuration data.
 * @author  Venca
 * @see     co_se_toho_tyka
 */
ConfigFile::ConfigFile(string name)
{
  fileName = name;

  // mutex for avoiding of race-condition with 
  // manipulating with queue
  pthread_mutex_init(&mutex, NULL);

  // Loads data config file
  if(Load() == KO)
  {
    exit(1);
  }
}

/**
 * Config file destructor.
 *
 * Destroys mutex that was used to protect access to STL map container.
 *
 * @author  Venca
 * @see     ConfigFile
 */
ConfigFile::~ConfigFile()
{
  pthread_mutex_destroy(&mutex);
}

/**
 * Returns value asociated vith name
 *
 * Looks the key in STL map container and returns the associated value,
 * that was specified in configuration file like this:
 * <pre>
 * key1 value1
 * key2 value2
 * </pre>
 *
 * @param   name    String key that we want to look up in configuration file.
 * @return  String that is associated with the specified key.
 * @author  Venca
 * @see     Load()
 */
string ConfigFile::GetValue(string name)
{
  pthread_mutex_lock(&mutex);
  if(cfgVariables[name].empty())
    fprintf(stderr,"%s not in config file\n", name.c_str());
  string value = cfgVariables[name];
  pthread_mutex_unlock(&mutex);
  return value;
}


/**
 * Lists all variables to stdout.
 *
 * Prints all the pairs key -> value from STL map  to stdout. Just
 * for debugging purposes.
 *
 * @author  Venca
 * @see     Load()
 */
void ConfigFile::PrintAll()
{
  pthread_mutex_lock(&mutex);
  CfgVariables::iterator pos = cfgVariables.begin();

  //printf("Promenne nactene ze souboru %s:\n", fileName.c_str());
  for(;pos != cfgVariables.end();pos++)
  {
    CfgVariable var;   
    const char * name = (*pos).first.c_str();
    const char * value = (*pos).second.c_str();
    fprintf(stderr, ":%s: = :%s:\n",name,value);
  }
  pthread_mutex_unlock(&mutex);
}

/**
 * Loads settings from config file.
 *
 * Opens the file with O_SHLOCK so that noone can modify the file as it is 
 * read. The configuration file is read into memory - it is really small
 * and has to be loaded anyway. And then the strings are parsed and
 * stored in a STL map container as pairs key -> value.
 *
 * @return  OK if success else KO
 * @author  Venca
 * @see     ReLoad()
 */
Err ConfigFile::Load()
{
  int fid;
  if((fid = open(fileName.c_str(),O_RDONLY|O_SHLOCK)) < 0) 
  {
    return KO;
  }
  struct stat *sb;
  sb = (struct stat *)malloc(sizeof(struct stat));
  fstat(fid, sb);
  int len = sb->st_size;

  CfgVariable cfgVariable;

  char *config = new char[len];
  char *ptr = config;
  char *start;
  string key;
  string value;

  // read in the file
  if( read(fid, ptr, len) < len )
    fprintf(stderr, "Error reading config file.\n");

  // lock access to cfgVariables when modifying it.
  pthread_mutex_lock(&mutex);

  cfgVariables.erase(cfgVariables.begin(),cfgVariables.end());

  while( ptr < config + len )
  {
     // Skip comments
    if(*ptr == '#')
    {
      while(*ptr != '\n') {ptr++;}
      continue;
    }
        
    if(*ptr == '\n')
    {
      ptr++;
      continue;
    }

      // Parse line
    if(*ptr != ' ')
    { 
        // Find key
      start = ptr;
      while(*(++ptr) != ' ') {}
      *ptr = '\0';  
      key = string(start);
        // Find Value
      start = ptr + 1;
      while(*(++ptr) != '\n') {}
      *ptr = '\0';
      ptr++;
      value = string(start);

        // Put the pair into structure
      cfgVariables[key] = value;
      continue;
    }
    else
    {
      ptr++;
    }
  }
  // unlock access to cfgVariables
  pthread_mutex_unlock(&mutex);

  close(fid);
  
  free(sb);
  delete[] config;
  return OK;
}

/**
 * Reloads settings from config file.
 *
 * Calls method Load that once more processes the configuration file.
 *
 *
 * @return  OK if success else KO.
 * @author  Venca
 * @see     Load()
 */
Err ConfigFile::Reload()
{
  return Load();
}
