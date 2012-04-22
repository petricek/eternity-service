#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <pthread.h>

#include <stdio.h>
#include <string>
#include <map>
#include "./../../H/common.h"


  /// Config variable
  typedef map<string, string, less<string> > CfgVariables; 
  /// Array of config variables
  typedef CfgVariables::value_type CfgVariable;

/**
 * Configuration file.
 *
 * Encapsulates work with configuration file. When created requires
 * config name - reads it in memory and then is ready to answer questions
 * about keys and values defined.
 * It can also be reloaded.
 *
 * @author  Venca
 * @see     Mix Acs Eso Client Bank
 */
class ConfigFile {

  private:

  /**@name attributes */
  /*@{*/
    /// Name of configuration file
    string fileName;
    /// Mutex to protect internal structures.
    pthread_mutex_t mutex;

    /// Array of pairs of name and value. 
    CfgVariables cfgVariables; 
  /*@}*/

  /**@name methods */
  /*@{*/
    /// Loads settings from file.
    Err Load();
    /// Prints variable var to stdout.
    Err PrintVar(CfgVariable *var);
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
    /// Opens and reads config from file.
    ConfigFile(string fileName);
    /// Destructor
    ~ConfigFile();

    /// Returns value of variable called "name".
    string GetValue(string name);
    /// Reloads settings from file.
    Err Reload();
    /// prints all variables to stdout.
    void PrintAll(); 
  /*@}*/

};

#endif
