/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_cui.c : character-based user interface and scripting */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_uims_inside.h"
#include "autogl_graphics.h"
#include "autogl_graphics_inside.h"
#include "autogl_gui.h"
#include "autogl_gui_inside.h"
#include "autogl_callback_inside.h"
#include "autogl_cui.h"



/* Module Variables */

#define MAX_WORD_LENGTH 80
#define MAX_ARGUMENTS 10

#define AUTOGL_METAFILE_NAME "autogl_metafile"

typedef enum {
  COMMAND_NONE,
  COMMAND_SET,
  COMMAND_PRINT,
  COMMAND_CALL,
  COMMAND_RUN,
  COMMAND_CLICK,
  COMMAND_DRAG,
  COMMAND_KEY,
  COMMAND_RESIZE
} CommandStatusType;

typedef struct {
  FILE *in;
  FILE *out;
  int isInteractive;
  int lineNumber;
  CommandStatusType commandStatus;
  int nArguments;
  char arguments[MAX_ARGUMENTS][MAX_WORD_LENGTH];
} Interpreter;



static AutoGL_Property *FindVariableProperty (char *name)
{
  int nPropertys;
  int iProperty;
  
  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    AutoGL_Property *property = AutoGL_INSIDE_GetProperty (iProperty);
    
    if (property->type == AUTOGL_PROPERTY_BOOLEAN
	|| property->type == AUTOGL_PROPERTY_STRING
	|| property->type == AUTOGL_PROPERTY_INTEGER
	|| property->type == AUTOGL_PROPERTY_REAL) {
      if (strcmp (property->name, name) == 0) {
	return property;
      }
    }
  }

  return NULL;
}

static AutoGL_Property *FindCallbackProperty (char *name)
{
  int nPropertys;
  int iProperty;
  
  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    AutoGL_Property *property = AutoGL_INSIDE_GetProperty (iProperty);
    
    if (property->type == AUTOGL_PROPERTY_CALLBACK) {
      if (strcmp (property->name, name) == 0) {
	return property;
      }
    }
  }

  return NULL;
}



/* Interpreter */

static void Interpreter_initialize 
(Interpreter *interpreter,
 FILE *in, FILE *out, int isInteractive)
{
  interpreter->in = in;
  interpreter->out = out;
  interpreter->isInteractive = isInteractive;
  interpreter->lineNumber = 0;
  interpreter->commandStatus = COMMAND_NONE;
  interpreter->nArguments = 0;
}

static void Interpreter_printError 
(Interpreter *interpreter)
{
  fprintf (stderr, "ERROR ");
  if (!interpreter->isInteractive) {
    fprintf (stderr, "line %d ", 
	     interpreter->lineNumber);
  }
  fprintf (stderr, "\n");
}

static void Interpreter_printPrompt 
(Interpreter *interpreter)
{
  if (interpreter->isInteractive) {
    fprintf (stderr, "!");
  }
}

static void Interpreter_performPrintCommand 
(Interpreter *interpreter, 
 char *variableName)
{
  AutoGL_Property *property;

  property = FindVariableProperty (variableName);
  if (property == NULL) {
    Interpreter_printError 
      (interpreter);
    fprintf (stderr, "variable property %s not found\n",
	     variableName);
    return;
  }

  switch (property->type) {
  case AUTOGL_PROPERTY_GROUP:
  case AUTOGL_PROPERTY_COMMENT:
  case AUTOGL_PROPERTY_CALLBACK:
    assert(0);
    break;
  case AUTOGL_PROPERTY_BOOLEAN:
    {
      int value = *property->booleanValue;

      if (value) {
	fprintf (interpreter->out, "true\n");
      } else {
	fprintf (interpreter->out, "false\n");
      }
    }
    break;
  case AUTOGL_PROPERTY_STRING:
    fprintf (interpreter->out, "\"%s\"\n", property->stringValue);
    break;
  case AUTOGL_PROPERTY_INTEGER:
    if (0 < property->nItems) {
      int value = *property->integerValue;
      
      fprintf (interpreter->out, "%d \"%s\"\n", 
	       value, property->itemLabels[value]);
    } else {
      fprintf (interpreter->out, "%d\n", *property->integerValue);
    }
    break;
  case AUTOGL_PROPERTY_REAL:
    fprintf (interpreter->out, "%f\n", *property->realValue);
    break;
  default:
    assert(0);
    break;
  }
}

static void Interpreter_performSetCommand 
(Interpreter *interpreter,
 char *variableName, char *valueString)
{
  AutoGL_Property *property;

  property = FindVariableProperty (variableName);
  if (property == NULL) {
    Interpreter_printError 
      (interpreter);
    fprintf (stderr, "variable property %s not found\n",
	     variableName);
    return;
  }

  switch (property->type) {
  case AUTOGL_PROPERTY_GROUP:
  case AUTOGL_PROPERTY_COMMENT:
  case AUTOGL_PROPERTY_CALLBACK:
    assert(0);
    break;
  case AUTOGL_PROPERTY_BOOLEAN:
    {
      if (strcmp (valueString, "true") == 0) {
	*property->booleanValue = 1;
      } else if (strcmp (valueString, "false") == 0) {
	*property->booleanValue = 0;
      } else {
	Interpreter_printError 
	  (interpreter);
	fprintf (stderr, "value must be true or false\n");
      }
    }
  break;
  case AUTOGL_PROPERTY_STRING:
    AutoGL_INSIDE_SetPropertyString (property, valueString);
    break;
  case AUTOGL_PROPERTY_INTEGER:
    if (0 < property->nItems) {
      int iItem;
      int found = 0;

      for (iItem = 0; iItem < property->nItems; iItem++) {
	if (strcmp (property->itemLabels[iItem], valueString) == 0) {
	  *property->integerValue = iItem;
	  found = 1;
	  break;
	}
      }
      if (!found) {
	Interpreter_printError 
	  (interpreter);
	fprintf (stderr, "value is not found in item list\n");
      }
    } else {
      int flag;
      int value;
      
      flag = sscanf (valueString, "%d", &value);
      if (flag == 1) {
	*property->integerValue = value;
      } else {
	Interpreter_printError 
	  (interpreter);
	fprintf (stderr, "value must be integer number\n");
      }
    }
    break;
  case AUTOGL_PROPERTY_REAL:
    {
      int flag;
      double value;
      
      flag = sscanf (valueString, "%lf", &value);
      if (flag == 1) {
	*property->realValue = value;
      } else {
	Interpreter_printError 
	  (interpreter);
	fprintf (stderr, "value must be real number\n");
      }
    }
    break;
  default:
    assert(0);
    break;
  }
}

static void Interpreter_performCallCommand 
(Interpreter *interpreter,
 char *callbackName)
{
  AutoGL_Property *property;
  
  property = FindCallbackProperty (callbackName);
  if (property == NULL) {
    Interpreter_printError 
      (interpreter);
    fprintf (stderr, "callback property %s not found\n",
	     callbackName);
    return;
  }

  AutoGL_INSIDE_InvokeCallback (property);
}

static void Interpreter_performRunCommand 
(Interpreter *interpreter,
 char *fileName)
{
  FILE *in;

  in = fopen (fileName, "r");
  if (in == NULL) {
    Interpreter_printError 
      (interpreter);
    fprintf (stderr, " file %s not found \n",
	     fileName);
    return;
  }
  
  AutoGL_InterpretInBatchMode (in, interpreter->out);
  
  fclose (in);
}

static void Interpreter_performInfoCommand 
(Interpreter *interpreter)
{
  int nPropertys;
  int iProperty;
  int iItem;
  
  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    AutoGL_Property *property = AutoGL_INSIDE_GetProperty (iProperty);
    
    switch (property->type) {
    case AUTOGL_PROPERTY_GROUP:
      fprintf (interpreter->out, " group ");
      break;
    case AUTOGL_PROPERTY_COMMENT:
      fprintf (interpreter->out, " comment ");
      break;
    case AUTOGL_PROPERTY_CALLBACK:
      fprintf (interpreter->out, " callback ");
      break;
    case AUTOGL_PROPERTY_BOOLEAN:
      fprintf (interpreter->out, " boolean ");
      break;
    case AUTOGL_PROPERTY_STRING:
      fprintf (interpreter->out, " string ");
      break;
    case AUTOGL_PROPERTY_INTEGER:
      fprintf (interpreter->out, " integer ");
      break;
    case AUTOGL_PROPERTY_REAL:
      fprintf (interpreter->out, " real ");
      break;
    default:
      assert(0);
      break;
    }
    if (strlen (property->name) == 0) {
      fprintf (interpreter->out, " name NO_NAME ");
    } else {
      fprintf (interpreter->out, " name %s ", property->name);
    }

    fprintf (interpreter->out, " label \"%s\" ", property->label);
    if (property->type == AUTOGL_PROPERTY_INTEGER) {
      fprintf (interpreter->out, " nItems %d ", property->nItems);
    }
    fprintf (interpreter->out, "\n");

    if (property->type == AUTOGL_PROPERTY_INTEGER) {
      for (iItem = 0; iItem < property->nItems; iItem++) {
	fprintf (interpreter->out, " itemLabel \"%s\" ", 
		 property->itemLabels[iItem]);
	fprintf (interpreter->out, "\n");
      }
    }
  }
}

static void Interpreter_performValueCommand 
(Interpreter *interpreter)
{
  int nPropertys;
  int iProperty;
  
  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    AutoGL_Property *property = AutoGL_INSIDE_GetProperty (iProperty);
    
    switch (property->type) {
    case AUTOGL_PROPERTY_GROUP:
      fprintf (interpreter->out, " group ");
      break;
    case AUTOGL_PROPERTY_COMMENT:
      fprintf (interpreter->out, " comment ");
      break;
    case AUTOGL_PROPERTY_CALLBACK:
      fprintf (interpreter->out, " callback ");
      break;
    case AUTOGL_PROPERTY_BOOLEAN:
      fprintf (interpreter->out, " boolean ");
      break;
    case AUTOGL_PROPERTY_STRING:
      fprintf (interpreter->out, " string ");
      break;
    case AUTOGL_PROPERTY_INTEGER:
      fprintf (interpreter->out, " integer ");
      break;
    case AUTOGL_PROPERTY_REAL:
      fprintf (interpreter->out, " real ");
      break;
    default:
      assert(0);
      break;
    }
    if (strlen (property->name) == 0) {
      fprintf (interpreter->out, " name NO_NAME ");
    } else {
      fprintf (interpreter->out, " name %s ", property->name);
    }

    switch (property->type) {
    case AUTOGL_PROPERTY_GROUP:
    case AUTOGL_PROPERTY_COMMENT:
    case AUTOGL_PROPERTY_CALLBACK:
      break;
    case AUTOGL_PROPERTY_BOOLEAN:
      {
	int value = *property->booleanValue;

	if (value) {
	  fprintf (interpreter->out, " value true ");
	} else {
	  fprintf (interpreter->out, " value false ");
	}
      }
      break;
    case AUTOGL_PROPERTY_STRING:
      fprintf (interpreter->out, " value \"%s\" ", property->stringValue);
      break;
    case AUTOGL_PROPERTY_INTEGER:
      if (0 < property->nItems) {
	int value = *property->integerValue;
	
	fprintf (interpreter->out, " value %d \"%s\" ", 
		 value, property->itemLabels[value]);
      } else {
	fprintf (interpreter->out, " value %d ", *property->integerValue);
      }
      break;
    case AUTOGL_PROPERTY_REAL:
      fprintf (interpreter->out, " value %f ", *property->realValue);
      break;
    default:
      assert(0);
      break;
    }

    fprintf (interpreter->out, "\n");

  }
}

static void GetDcOfMouse 
(int *dcX_OUT, int *dcY_OUT, int *dcZ_OUT,
 int mouseX, int mouseY)
{
  int widthDc, heightDc;
  
  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);
  *dcX_OUT = mouseX - widthDc / 2;
  *dcY_OUT = heightDc / 2 - mouseY;
  *dcZ_OUT = 0;
}

static void Interpreter_performClickCommand 
(Interpreter *interpreter,
 char *xStr, char *yStr)
{
  int x = atoi (xStr);
  int y = atoi (yStr);
  int dcX, dcY, dcZ;
    
  GetDcOfMouse 
    (&dcX, &dcY, &dcZ,
     x, y);
  AutoGL_INSIDE_SetPointingDeviceEvent 
    (AUTOGL_EVENT_POINTING_DEVICE_PRESS, 
     dcX, dcY, dcZ);
  AutoGL_INSIDE_InvokeViewEventCallback ();

  AutoGL_INSIDE_SetPointingDeviceEvent 
    (AUTOGL_EVENT_POINTING_DEVICE_RELEASE, 
     dcX, dcY, dcZ);
  AutoGL_INSIDE_InvokeViewEventCallback ();
}

static void Interpreter_performDragCommand 
(Interpreter *interpreter,
 char *startXStr, char *startYStr,
 char *endXStr, char *endYStr)
{
  int startX = atoi (startXStr);
  int startY = atoi (startYStr);
  int endX = atoi (endXStr);
  int endY = atoi (endYStr);
  int dcX, dcY, dcZ;
    
  GetDcOfMouse 
    (&dcX, &dcY, &dcZ,
     startX, startY);
  AutoGL_INSIDE_SetPointingDeviceEvent 
    (AUTOGL_EVENT_POINTING_DEVICE_PRESS, 
     dcX, dcY, dcZ);
  AutoGL_INSIDE_InvokeViewEventCallback ();
  
  GetDcOfMouse 
    (&dcX, &dcY, &dcZ,
     endX, endY);
  AutoGL_INSIDE_SetPointingDeviceEvent 
    (AUTOGL_EVENT_POINTING_DEVICE_DRAG, 
     dcX, dcY, dcZ);
  AutoGL_INSIDE_InvokeViewEventCallback ();

  AutoGL_INSIDE_SetPointingDeviceEvent 
    (AUTOGL_EVENT_POINTING_DEVICE_RELEASE, 
     dcX, dcY, dcZ);
  AutoGL_INSIDE_InvokeViewEventCallback ();
}

static void Interpreter_performKeyCommand 
(Interpreter *interpreter,
 char *keyStr)
{
  int keyChar = *keyStr;
    
  /* NEED TO REDEFINE */
  /* non printable key is not supported */
  /* enter, tab, shift, ctrl, etc. */

  AutoGL_INSIDE_SetKeyEvent (keyChar);
  AutoGL_INSIDE_InvokeViewEventCallback ();
}

static void Interpreter_performResizeCommand 
(Interpreter *interpreter,
 char *widthStr, char *heightStr)
{
  int widthDc = atoi (widthStr);
  int heightDc = atoi (heightStr);

  /* NEED TO REDEFINE */
  /* AutoGL-MP OS-X */
  /* broadcast resizing */

  AutoGL_ResizeView (widthDc, heightDc);
}

static void Interpreter_performHelpCommand 
(Interpreter *interpreter)
{
  fprintf (interpreter->out, "Command List\n");
  fprintf (interpreter->out, "\n");
  fprintf (interpreter->out, "info \n");
  fprintf (interpreter->out, "\t list all the variables and callbacks \n");
  fprintf (interpreter->out, "value \n");
  fprintf (interpreter->out, "\t list the value for each variable \n");
  fprintf (interpreter->out, "set <variable name> <value> \n");
  fprintf (interpreter->out, "\t set the value of the variable as <value> \n");
  fprintf (interpreter->out, "print <variable name> \n");
  fprintf (interpreter->out, "\t print the value of the variable \n");
  fprintf (interpreter->out, "call <callback name> \n");
  fprintf (interpreter->out, "\t call callback function \n");
  fprintf (interpreter->out, "run <script file name> \n");
  fprintf (interpreter->out, "\t run and interpret the script file \n");
  fprintf (interpreter->out, "draw \n");
  fprintf (interpreter->out, "\t redraw view \n");
  fprintf (interpreter->out, "resize <width> <height> \n");
  fprintf (interpreter->out, "\t resize view window \n");
  fprintf (interpreter->out, "click <x> <y> \n");
  fprintf (interpreter->out, "\t click at (x, y) \n");
  fprintf (interpreter->out, "drag <x0> <y0> <x1> <y1> \n");
  fprintf (interpreter->out, "\t drag from (x0, y0) to (x1, y1) \n");
  fprintf (interpreter->out, "key <key character> \n");
  fprintf (interpreter->out, "\t type keyboard <key character> \n");
  fprintf (interpreter->out, "write \n");
  fprintf (interpreter->out, "\t write display list into AutoGL metafile \n");
  fprintf (interpreter->out, "help \n");
  fprintf (interpreter->out, "\t print this message  \n");
  fprintf (interpreter->out, "exit | quit \n");
  fprintf (interpreter->out, "\t return from this interpretation mode  \n");
  fprintf (interpreter->out, "\n");
}

static void Interpreter_putCommand 
(Interpreter *interpreter,
 char *word)
{
  if (strcmp (word, "set") == 0) {
    interpreter->commandStatus = COMMAND_SET;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "print") == 0) {
    interpreter->commandStatus = COMMAND_PRINT;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "call") == 0) {
    interpreter->commandStatus = COMMAND_CALL;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "run") == 0) {
    interpreter->commandStatus = COMMAND_RUN;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "draw") == 0) {
    AutoGL_DrawView ();
    interpreter->commandStatus = COMMAND_NONE;
    interpreter->nArguments = 0;
    Interpreter_printPrompt 
      (interpreter);
  } else if (strcmp (word, "info") == 0) {
    Interpreter_performInfoCommand 
      (interpreter);
    interpreter->commandStatus = COMMAND_NONE;
    interpreter->nArguments = 0;
    Interpreter_printPrompt 
      (interpreter);
  } else if (strcmp (word, "value") == 0) {
    Interpreter_performValueCommand 
      (interpreter);
    interpreter->commandStatus = COMMAND_NONE;
    interpreter->nArguments = 0;
    Interpreter_printPrompt 
      (interpreter);
  } else if (strcmp (word, "click") == 0) {
    interpreter->commandStatus = COMMAND_CLICK;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "drag") == 0) {
    interpreter->commandStatus = COMMAND_DRAG;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "key") == 0) {
    interpreter->commandStatus = COMMAND_KEY;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "resize") == 0) {
    interpreter->commandStatus = COMMAND_RESIZE;
    interpreter->nArguments = 0;
  } else if (strcmp (word, "write") == 0) {
    AutoGL_WriteMetafile (AUTOGL_METAFILE_NAME);
    interpreter->commandStatus = COMMAND_NONE;
    interpreter->nArguments = 0;
    Interpreter_printPrompt 
      (interpreter);
  } else if (strcmp (word, "help") == 0) {
    Interpreter_performHelpCommand 
      (interpreter);
    interpreter->commandStatus = COMMAND_NONE;
    interpreter->nArguments = 0;
    Interpreter_printPrompt 
      (interpreter);
  } else {
    Interpreter_printError 
      (interpreter);
    fprintf (stderr, " udefined command \n");
    Interpreter_printPrompt 
      (interpreter);
  }
}

static void Interpreter_putArgument 
(Interpreter *interpreter,
 char *word)
{
  int commandIsDone;

  if (MAX_ARGUMENTS <= interpreter->nArguments) return;

  strcpy (interpreter->arguments[interpreter->nArguments], word);
  interpreter->nArguments++;
  
  commandIsDone = 0;
  switch (interpreter->commandStatus) {
  case COMMAND_NONE:
    assert(0);
    break;
  case COMMAND_SET:
    if (interpreter->nArguments == 2) {
      Interpreter_performSetCommand 
	(interpreter,
	 interpreter->arguments[0], interpreter->arguments[1]);
      commandIsDone = 1;

      /* NEED TO REDEFINE */
      /* update panel */
    }
    break;
  case COMMAND_PRINT:
    if (interpreter->nArguments == 1) {
      Interpreter_performPrintCommand 
	(interpreter,
	 interpreter->arguments[0]);
      commandIsDone = 1;
    }
    break;
  case COMMAND_CALL:
    if (interpreter->nArguments == 1) {
      Interpreter_performCallCommand 
	(interpreter,
	 interpreter->arguments[0]);
      commandIsDone = 1;
    }
    break;
  case COMMAND_RUN:
    if (interpreter->nArguments == 1) {
      Interpreter_performRunCommand 
	(interpreter,
	 interpreter->arguments[0]);
      commandIsDone = 1;
    }
    break;
  case COMMAND_CLICK:
    if (interpreter->nArguments == 2) {
      Interpreter_performClickCommand 
	(interpreter,
	 interpreter->arguments[0], 
	 interpreter->arguments[1]);
      commandIsDone = 1;
    }
    break;
  case COMMAND_DRAG:
    if (interpreter->nArguments == 4) {
      Interpreter_performDragCommand 
	(interpreter,
	 interpreter->arguments[0], 
	 interpreter->arguments[1],
	 interpreter->arguments[2], 
	 interpreter->arguments[3]);
      commandIsDone = 1;
    }
    break;
  case COMMAND_KEY:
    if (interpreter->nArguments == 1) {
      Interpreter_performKeyCommand 
	(interpreter,
	 interpreter->arguments[0]);
      commandIsDone = 1;
    }
    break;
  case COMMAND_RESIZE:
    if (interpreter->nArguments == 2) {
      Interpreter_performResizeCommand 
	(interpreter,
	 interpreter->arguments[0], 
	 interpreter->arguments[1]);
      commandIsDone = 1;
    }
    break;
  default:
    assert(0);
    break;
  }

  if (commandIsDone) {
    interpreter->commandStatus = COMMAND_NONE;
    interpreter->nArguments = 0;
    Interpreter_printPrompt 
      (interpreter);
  }
}

static int Interpreter_isEndCommand 
(Interpreter *interpreter,
 char *word)
{
  if (interpreter->commandStatus == COMMAND_NONE) {
    if (strcmp (word, "exit") == 0
	|| strcmp (word, "quit") == 0) {
      return 1;
    } 
  }
  return 0;
}

static void Interpreter_putWord 
(Interpreter *interpreter,
 char *word)
{
  if (interpreter->commandStatus == COMMAND_NONE) {
    Interpreter_putCommand 
      (interpreter, 
       word);
  } else {
    Interpreter_putArgument 
      (interpreter, 
       word);
  }
}

static void Interpreter_perform 
(Interpreter *interpreter)
{
  char word[MAX_WORD_LENGTH];
  int wordLength;
  int isString;

  Interpreter_printPrompt 
    (interpreter);

  wordLength = 0;
  strcpy (word, "");
  isString = 0;

  for (;;) {
    int ch = fgetc (interpreter->in);
    if (ch == EOF) break;

    if (ch == '"') {
      if (isString) {
	/* the end of quotation */
	isString = 0;

	Interpreter_putWord 
	  (interpreter, 
	   word);
	wordLength = 0;
	word[wordLength] = 0;
      } else {
	/* the beginning of quotation */
	isString = 1;
      }
    } else {
      if (isString) {
	/* inside quotation */

	if (wordLength < MAX_WORD_LENGTH - 1) {
	  word[wordLength] = ch;
	  wordLength++;
	  word[wordLength] = 0;
	}
      } else {
	/* outside quotation */

	if (isspace (ch)) {
	  if (Interpreter_isEndCommand 
	      (interpreter, 
	       word)) {
	    return;
	  }
	  if (0 < wordLength) {
	    Interpreter_putWord 
	      (interpreter, 
	       word);
	  }
	  wordLength = 0;
	  word[wordLength] = 0;
	} else {
	  if (wordLength < MAX_WORD_LENGTH - 1) {
	    word[wordLength] = ch;
	    wordLength++;
	    word[wordLength] = 0;
	  }
	}
      }
    }

    if (ch == '\n') {
      interpreter->lineNumber++;
    }
  }
}



void AutoGL_InterpretInBatchMode (FILE *in, FILE *out)
{
  Interpreter interpreter;
  int isInteractive = 0;

  Interpreter_initialize 
    (&interpreter, 
     in, out, isInteractive);
  Interpreter_perform 
    (&interpreter);
}

void AutoGL_InterpretInInteractiveMode (void)
{
  Interpreter interpreter;
  int isInteractive = 1;

  Interpreter_initialize 
    (&interpreter, 
     stdin, stdout, isInteractive);
  Interpreter_perform 
    (&interpreter);
}

void AutoGL_InterpretScript (char *fileName)
{
  Interpreter interpreter;
  int isInteractive = 0;
  FILE *in;

  in = fopen (fileName, "r");
  if (in == NULL) {
    fprintf (stderr, " file %s not found \n",
	     fileName);
    return;
  }

  Interpreter_initialize 
    (&interpreter, 
     in, stdout, isInteractive);
  Interpreter_perform 
    (&interpreter);

  fclose (in);
}

