/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_com.c : communication using AutoNOSim */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <autonosim.h>

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_uims_inside.h"
#include "autogl_graphics.h"
#include "autogl_graphics_inside.h"
#include "autogl_gui.h"
#include "autogl_gui_inside.h"
#include "autogl_com_inside.h"



#if 0
#define MSG
#endif



/* Module Variables */

#define AUTOGL_COM_COMMAND_REDRAW 0
#define AUTOGL_COM_COMMAND_CALLBACK 1
#define AUTOGL_COM_COMMAND_IDLE 2
#define AUTOGL_COM_COMMAND_MOUSE 3
#define AUTOGL_COM_COMMAND_KEY 4
#define AUTOGL_COM_COMMAND_QUIT 5
#define AUTOGL_COM_COMMAND_RESIZE 6

static int NTasks;
static int MyTaskId;
static int IsSynchronized;



void AutoGL_INSIDE_InitializeAutoNOSim (int *argc, char ***argv)
{
  AutoNOSim_Initialize (argc, argv);
  
  NTasks = AutoNOSim_NTasks ();
  MyTaskId = AutoNOSim_MyTaskId ();
}

void AutoGL_INSIDE_TerminateAutoNOSim (void)
{
  AutoNOSim_Terminate ();
}

static void SendReceivePropertyArguments (void)
{
  int nPropertys;
  int iProperty;
  
  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    AutoGL_Property *property = AutoGL_INSIDE_GetProperty (iProperty);
    
    switch (property->type) {
    case AUTOGL_PROPERTY_GROUP:
    case AUTOGL_PROPERTY_COMMENT:
    case AUTOGL_PROPERTY_CALLBACK:
      break;
    case AUTOGL_PROPERTY_BOOLEAN:
      AutoNOSim_Integer (property->booleanValue);
      break;
    case AUTOGL_PROPERTY_STRING:
      AutoNOSim_String (property->stringValue, property->stringLength);
      break;
    case AUTOGL_PROPERTY_INTEGER:
      AutoNOSim_Integer (property->integerValue);
      break;
    case AUTOGL_PROPERTY_REAL:
      AutoNOSim_Real (property->realValue);
      break;
    default:
      assert(0);
      break;
    }
  }
}



/* REDRAW function */

static double REDRAW_Size;
static double REDRAW_CenterX;
static double REDRAW_CenterY;
static double REDRAW_CenterZ;
static double REDRAW_DirectionX;
static double REDRAW_DirectionY;
static double REDRAW_DirectionZ;
static double REDRAW_UpVectorX;
static double REDRAW_UpVectorY;
static double REDRAW_UpVectorZ;

static void REDRAW_Arguments (void)
{
  AutoNOSim_Real (&REDRAW_Size);
  AutoNOSim_Real (&REDRAW_CenterX);
  AutoNOSim_Real (&REDRAW_CenterY);
  AutoNOSim_Real (&REDRAW_CenterZ);
  AutoNOSim_Real (&REDRAW_DirectionX);
  AutoNOSim_Real (&REDRAW_DirectionY);
  AutoNOSim_Real (&REDRAW_DirectionZ);
  AutoNOSim_Real (&REDRAW_UpVectorX);
  AutoNOSim_Real (&REDRAW_UpVectorY);
  AutoNOSim_Real (&REDRAW_UpVectorZ);

  SendReceivePropertyArguments ();
}

static void REDRAW_Body (void)
{
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

  AutoGL_SetViewSize (REDRAW_Size);
  AutoGL_SetViewCenter (REDRAW_CenterX, 
			REDRAW_CenterY, 
			REDRAW_CenterZ);
  AutoGL_SetViewDirection (REDRAW_DirectionX, 
			   REDRAW_DirectionY, 
			   REDRAW_DirectionZ);
  AutoGL_SetViewUpVector (REDRAW_UpVectorX, 
			  REDRAW_UpVectorY, 
			  REDRAW_UpVectorZ);

#ifdef MSG
  fprintf (stderr, " [taskId %d] redraw event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_UpdateView ();
}

void AutoGL_INSIDE_SendRedrawEvent (void)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_REDRAW;
  AutoNOSim_Integer (&commandId);

  REDRAW_Size = AutoGL_GetViewSize ();
  AutoGL_GetViewCenter (&REDRAW_CenterX, 
			&REDRAW_CenterY, 
			&REDRAW_CenterZ);
  AutoGL_GetViewDirection (&REDRAW_DirectionX, 
			   &REDRAW_DirectionY, 
			   &REDRAW_DirectionZ);
  AutoGL_GetViewUpVector (&REDRAW_UpVectorX, 
			  &REDRAW_UpVectorY, 
			  &REDRAW_UpVectorZ);
  REDRAW_Arguments ();

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] redraw event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallViewRedrawCallback ();
}



/* CALLBACK function */

static int CALLBACK_PropertyId;

static void CALLBACK_Arguments (void)
{
  AutoNOSim_Integer (&CALLBACK_PropertyId);

  SendReceivePropertyArguments ();
}

static void CALLBACK_Body (void)
{
  AutoGL_Property *property;
  
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

  property = AutoGL_INSIDE_GetProperty (CALLBACK_PropertyId);

#ifdef MSG
  fprintf (stderr, " [taskId %d] callback event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallCallback (property);
}

static int GetPropertyId (AutoGL_Property *property)
{
  int nPropertys;
  int iProperty;
  
  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    if (property == AutoGL_INSIDE_GetProperty (iProperty)) {
      return iProperty;
    }
  }
  
  assert(0);
  return -1;
}

void AutoGL_INSIDE_SendCallbackEvent (AutoGL_Property *property)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);
  assert(property->type == AUTOGL_PROPERTY_CALLBACK);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_CALLBACK;
  AutoNOSim_Integer (&commandId);

  CALLBACK_PropertyId = GetPropertyId (property);
  CALLBACK_Arguments ();

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] callback event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallCallback (property);
}



/* IDLE function */

static void IDLE_Arguments (void)
{
}

static void IDLE_Body (void)
{
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

#ifdef MSG
  fprintf (stderr, " [taskId %d] idle event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();
  
  AutoGL_INSIDE_CallIdleEventCallback ();
}

void AutoGL_INSIDE_SendIdleEvent (void)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_IDLE;
  AutoNOSim_Integer (&commandId);

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] idle event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallIdleEventCallback ();
}



/* MOUSE function */

static int MOUSE_event;
static int MOUSE_dcX;
static int MOUSE_dcY;
static int MOUSE_dcZ;

static void MOUSE_Arguments (void)
{
  AutoNOSim_Integer (&MOUSE_event);
  AutoNOSim_Integer (&MOUSE_dcX);
  AutoNOSim_Integer (&MOUSE_dcY);
  AutoNOSim_Integer (&MOUSE_dcZ);

  SendReceivePropertyArguments ();
}

static void MOUSE_Body (void)
{
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

  AutoGL_INSIDE_SetPointingDeviceEvent 
    (MOUSE_event, 
     MOUSE_dcX, MOUSE_dcY, MOUSE_dcZ);

#ifdef MSG
  fprintf (stderr, " [taskId %d] mouse event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallViewEventCallback ();
}

void AutoGL_INSIDE_SendMouseEvent (void)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_MOUSE;
  AutoNOSim_Integer (&commandId);

  MOUSE_event = AutoGL_GetViewEvent (); 
  AutoGL_GetPointingDevicePositionDc 
    (&MOUSE_dcX, &MOUSE_dcY, &MOUSE_dcZ);
  MOUSE_Arguments ();

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] mouse event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallViewEventCallback ();
}



/* KEY function */

static int KEY_keyChar;

static void KEY_Arguments (void)
{
  AutoNOSim_Integer (&KEY_keyChar);

  SendReceivePropertyArguments ();
}

static void KEY_Body (void)
{
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

  AutoGL_INSIDE_SetKeyEvent (KEY_keyChar);

#ifdef MSG
  fprintf (stderr, " [taskId %d] key event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallViewEventCallback ();
}

void AutoGL_INSIDE_SendKeyEvent (void)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_KEY;
  AutoNOSim_Integer (&commandId);

  KEY_keyChar = AutoGL_GetKeyChar (); 
  KEY_Arguments ();

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] key event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_CallViewEventCallback ();
}



/* QUIT function */

static void QUIT_Arguments (void)
{
}

static void QUIT_Body (void)
{
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

#ifdef MSG
  fprintf (stderr, " [taskId %d] quit \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_TerminateAutoNOSim ();

  exit (0);
}

void AutoGL_INSIDE_SendQuitEvent (void)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_QUIT;
  AutoNOSim_Integer (&commandId);

  QUIT_Arguments ();

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] quit event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_TerminateAutoNOSim ();
}



/* RESIZE function */

static int RESIZE_widthDc; 
static int RESIZE_heightDc; 

static void RESIZE_Arguments (void)
{
  AutoNOSim_Integer (&RESIZE_widthDc);
  AutoNOSim_Integer (&RESIZE_heightDc);
}

static void RESIZE_Body (void)
{
  assert(MyTaskId != AUTONOSIM_MASTER_TASK_ID);

#ifdef MSG
  fprintf (stderr, " [taskId %d] resize event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_ResizeView (RESIZE_widthDc, RESIZE_heightDc);
}

void AutoGL_INSIDE_SendResizeEvent (int widthDc, int heightDc)
{
  int iTask;
  int commandId;

  if (!AutoGL_INSIDE_IsSynchronized ()) return;

  assert(MyTaskId == AUTONOSIM_MASTER_TASK_ID);

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  commandId = AUTOGL_COM_COMMAND_RESIZE;
  AutoNOSim_Integer (&commandId);

  RESIZE_widthDc = widthDc; 
  RESIZE_heightDc = heightDc; 
  RESIZE_Arguments ();

  for (iTask = 0; iTask < NTasks; iTask++) {
    if (iTask != AUTONOSIM_MASTER_TASK_ID) {
      AutoNOSim_SendTo (iTask);
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] resize event \n",
	   MyTaskId);
#endif

  AutoNOSim_Barrier ();

  AutoGL_INSIDE_ResizeView (RESIZE_widthDc, RESIZE_heightDc);
}



/* Synchronization and Task Loop */

void AutoGL_INSIDE_Synchronize (void)
{
  IsSynchronized = 1;

  fprintf (stderr, 
	   " [taskId %d] synchronize \n",
	   MyTaskId);
  
  if (MyTaskId == AUTONOSIM_MASTER_TASK_ID) return;

  for (;;) {
    int commandId;

    AutoNOSim_ReceiveFrom (AUTONOSIM_MASTER_TASK_ID);
    AutoNOSim_Clear (AUTONOSIM_UNPACKING);

    AutoNOSim_Integer (&commandId);

    switch (commandId) {
    case AUTOGL_COM_COMMAND_REDRAW:
      REDRAW_Arguments ();
      REDRAW_Body ();
      break;
    case AUTOGL_COM_COMMAND_CALLBACK:
      CALLBACK_Arguments ();
      CALLBACK_Body ();
      break;
    case AUTOGL_COM_COMMAND_IDLE:
      IDLE_Arguments ();
      IDLE_Body ();
      break;
    case AUTOGL_COM_COMMAND_MOUSE:
      MOUSE_Arguments ();
      MOUSE_Body ();
      break;
    case AUTOGL_COM_COMMAND_KEY:
      KEY_Arguments ();
      KEY_Body ();
      break;
    case AUTOGL_COM_COMMAND_QUIT:
      QUIT_Arguments ();
      QUIT_Body ();
      break;
    case AUTOGL_COM_COMMAND_RESIZE:
      RESIZE_Arguments ();
      RESIZE_Body ();
      break;
    default:
      assert(0);
      break;
    }
  }
}

int AutoGL_INSIDE_IsSynchronized (void)
{
  return IsSynchronized;
}



/* Image Composition */

void AutoGL_INSIDE_SendTaskImage 
(int imageWidthDc, int imageHeightDc,
 unsigned char imageColors[], unsigned int imageDepths[])
{
  int nPixels = imageWidthDc * imageHeightDc;
  int colorSize = nPixels * sizeof (unsigned char) * 3;
  int depthSize = nPixels * sizeof (unsigned int);

  AutoNOSim_Barrier ();

  AutoNOSim_Clear (AUTONOSIM_PACKING);

  AutoNOSim_Integer (&colorSize);
  AutoNOSim_String ((char *)imageColors, colorSize);
  AutoNOSim_Integer (&depthSize);
  AutoNOSim_String ((char *)imageDepths, depthSize);

  AutoNOSim_SendTo (AUTONOSIM_MASTER_TASK_ID);

#ifdef MSG
  fprintf (stderr, " [taskId %d] send task image \n",
	   MyTaskId);
#endif
}

void AutoGL_INSIDE_GatherAndComposeImage 
(int imageWidthDc, int imageHeightDc,
 unsigned char imageColors[], unsigned int imageDepths[])
{
  static unsigned char receivedColors[MAX_PIXEL * 3];
  static unsigned int receivedDepths[MAX_PIXEL];
  int nPixels = imageWidthDc * imageHeightDc;
  int colorSize = nPixels * sizeof (unsigned char) * 3;
  int depthSize = nPixels * sizeof (unsigned int);
  int nTasks = AutoNOSim_NTasks ();
  int iTask;
  int iPixel;

  AutoNOSim_Barrier ();

  for (iTask = 0; iTask < nTasks; iTask++) {
    int dummy;

    if (iTask == AUTONOSIM_MASTER_TASK_ID) {
      continue;
    }

    AutoNOSim_ReceiveFrom (iTask);
    AutoNOSim_Clear (AUTONOSIM_UNPACKING);

    AutoNOSim_Integer (&dummy);
    assert(dummy == colorSize);
    AutoNOSim_String ((char *)receivedColors, colorSize);
    AutoNOSim_Integer (&dummy);
    assert(dummy == depthSize);
    AutoNOSim_String ((char *)receivedDepths, depthSize);

    for (iPixel = 0; iPixel < nPixels; iPixel++) {
      if (receivedDepths[iPixel] <= imageDepths[iPixel]) {
        imageDepths[iPixel] = receivedDepths[iPixel];

	imageColors[iPixel * 3 + 0] = receivedColors[iPixel * 3 + 0];
	imageColors[iPixel * 3 + 1] = receivedColors[iPixel * 3 + 1];
	imageColors[iPixel * 3 + 2] = receivedColors[iPixel * 3 + 2];
      }
    }
  }

#ifdef MSG
  fprintf (stderr, " [taskId %d] gather and compose image \n",
	   MyTaskId);
#endif
}

int AutoGL_INSIDE_GatherSelectedId (int selectedId, unsigned int minZ)
{
  int result = -1;

  AutoNOSim_Clear (AUTONOSIM_PACKING);
  AutoNOSim_Integer (&selectedId);
  AutoNOSim_Integer ((int *)&minZ);
  AutoNOSim_SendTo (AUTONOSIM_MASTER_TASK_ID);

  if (AutoNOSim_MyTaskId () == AUTONOSIM_MASTER_TASK_ID) {
    int nTasks = AutoNOSim_NTasks ();
    int iTask;
    int masterSelectedId = -1;
    unsigned int masterMinZ = 0;
    
    for (iTask = 0; iTask < nTasks; iTask++) {
      int slaveSelectedId;
      unsigned int slaveMinZ;
      
      AutoNOSim_ReceiveFrom (iTask);
      AutoNOSim_Clear (AUTONOSIM_UNPACKING);
      AutoNOSim_Integer (&slaveSelectedId);
      AutoNOSim_Integer ((int *)&slaveMinZ);

      if (0 <= slaveSelectedId) {
	if (masterSelectedId < 0
	    || slaveMinZ < masterMinZ) {
	  masterSelectedId = slaveSelectedId;
	  masterMinZ = slaveMinZ;
	}
      }
    }

    for (iTask = 0; iTask < nTasks; iTask++) {
      AutoNOSim_Clear (AUTONOSIM_PACKING);
      AutoNOSim_Integer (&masterSelectedId);
      AutoNOSim_SendTo (iTask);
    }
  }

  AutoNOSim_ReceiveFrom (AUTONOSIM_MASTER_TASK_ID);
  AutoNOSim_Clear (AUTONOSIM_UNPACKING);
  AutoNOSim_Integer (&result);

  return result;
}




