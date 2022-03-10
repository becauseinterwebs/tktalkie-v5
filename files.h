/****
 * Common file routines
 */

/**
 * Make sure the file has an extension
 */
void addFileExt(char *path)
{
  debug(F("Add ext %s to file %s\n"), FILE_EXT, path);
  if (path == NULL) {
    return;
  }
  char buf[FILENAME_SIZE];
  strcpy(buf, path);
  upcase(buf);
  char *ret = strstr(buf, FILE_EXT);  
  if (ret == NULL) {
    strcat(path, FILE_EXT);
  }
}

/**
 * Make sure file has backup extension
 */
void addBackupExt(char *path)
{
  char *ret = strstr(path, ".");  
  if (ret != NULL) {
    char *ptr;
    ret = strtok_r(path, ".", &ptr);
  }
  strcat(path, BACKUP_EXT);
}

/**
 * Makes sure paths start and end with /
 */
void fixPath(char *path)
{
   if (strcasecmp(path, "") == 0) {
    return;
   }
   if (path[0] != '/') {
     char buf[SETTING_ENTRY_MAX] = "/";
     strcat(buf, path);
     strcpy(path, buf);
   }
   int i = 1;
   while (path[i] != '\0' && i < SETTING_ENTRY_MAX) {
    i++;
   }
   if (path[i-1] != '/') {
    path[i]   = '/';
    path[i+1] = '\0';
   }
}

/**
 * Delete the specified file
 */
boolean deleteFile(const char *filename) 
{
  if (SD.exists(filename)) {
    SD.remove(filename);
    return true;
  } else {
    return false;
  }
}

/**
 * Open a file on the SD card in the specified mode.
 */
File openFile(const char *filename, int mode) 
{
  // Thanks to TanRu !
  if (mode == FILE_WRITE) {
    deleteFile(filename);
  }
  debug(F("Opening file %s\n"), filename);
  return SD.open(filename, mode);  
}

/***
 * Show a text file
 */
void showFile(const char *filename) {
  debug(F("Showing file %s\n"), filename);
  AudioNoInterrupts();
  File file = SD.open(filename);
  if (file) {
    char c;
    while (file.available()) {
      c = file.read();
      Serial.print(c);
    }
    file.close();
  } else {
    Serial.println(F("Could not find file!"));
  }
  AudioInterrupts();
}

/***
 * recursively list all files on SD card
 */
String dirSep = "";

int listDirectories(const char *path, char directories[][FILENAME_SIZE])
{
   int index = 0;
   File dir = SD.open(path);
   if (!dir) {
    debug(F("Could not open %s!\n"), path);
    return 0;
   }
   dir.rewindDirectory();
   debug(F("List directories in %s, %d max\n"), path, MAX_FILE_COUNT);
   while(true && index < MAX_FILE_COUNT+1) {
     File entry = dir.openNextFile();
     if (! entry) {
       // no more files
       debug(F("No more directories\n"));
       break;
     }
     if (entry.isDirectory()) {
       char *ret = strstr(entry.name(), "~");
       if (ret == NULL) {
         debug(F("%s\n"), entry.name());
         strcpy(directories[index], entry.name());
         //index += listDirectories(entry.name(), directories);
         index++;
       }
     }  
     entry.close();
   }
   debug(F("%d directories found\n"), index);
   return index;
}

/**
 * Return a directory listing.
 * If filter is specified, only file names containing
 * the filter text are returned.
 */
 int listFiles(const char *path, char files[][FILENAME_SIZE], int max, const char *match, boolean recurse, boolean echo) 
{
  char filter[FILENAME_SIZE];
  strcpy(filter, match);
  upcase(filter);
  boolean checkFilter = (strcasecmp(filter, "") == 0) ? false : true;
  if (checkFilter) {
    debug(F("Filter: %s\n"), filter);
  }
  int index = 0;
  if (!SD.exists(path)) {
    debug(F("Path %s not found!\n"), path);
    return 0;
  }
  AudioNoInterrupts();
  File dir = SD.open(path);
  if (!dir) {
    debug(F("Could not open %s!\n"), path);
    AudioInterrupts();
    return 0;
  }
  dir.rewindDirectory();
  while(true && index < max) {
     File entry = dir.openNextFile();
     if (! entry) {
       // no more files
       debug(F("No more files\n"));
       if (dirSep != "") {
          dirSep = dirSep.substring(0, dirSep.length()-2);
       }
       break;
     }
     if (entry.isDirectory() && recurse == true) {
       // Filter out folders with ~ (backups)
       char *ret = strstr(entry.name(), "~");
       if (ret == NULL) {
         if (echo || Config.debug == 1) {
          debug(F("%s%s/n"), dirSep, entry.name());
         }
         dirSep += "  ";
         index += listFiles(entry.name(), files, max-index, filter, recurse, echo);
       }
     } else {
        char *fname = entry.name();
        upcase(fname);
        // Filter out filenames with ~ (backups)
        char *ret = strstr(fname, "~");
        if (ret == NULL) {
            debug(F("%s is not a backup file\n"), entry.name());
            ret = strstr(fname, "._");
            if (ret == NULL) {
              debug(F("%s is not a system file\n"), entry.name());
              if (checkFilter) {
                ret = strstr(fname, filter);
                if (ret == NULL) {
                  fname[0] = '\0';
                }
              }
            }
        } else {
          fname[0] ='\0';
        }
        if (strcasecmp(fname, "") != 0) {
            if (echo || Config.debug == 1) {
              debug(F("%s%s\n"), dirSep, entry.name());
            }
            if (index < max) {
              strcpy(files[index], entry.name());
              index += 1;
            }
       }
     }
     entry.close();
   }
   AudioInterrupts();
   return index;
}
