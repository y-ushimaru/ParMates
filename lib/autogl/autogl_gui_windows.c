/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_gui_windows.c : Windows and OpenGL (using Cygwin) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "autogl_os.h"
#include "autogl_math.h"
#include "autogl_uims.h"
#include "autogl_uims_inside.h"
#include "autogl_graphics.h"
#include "autogl_graphics_inside.h"
#include "autogl_gui.h"
#include "autogl_gui_inside.h"

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
#include "autogl_com_inside.h"
#endif

#include "autogl_callback_inside.h"
#include "autogl_graphics_opengl_inside.h"

#include <windows.h>



/* Module Variables */

/* Windows instance of the application program */
static HINSTANCE TheInstance;
static int UnitWidth = 9;
static int UnitHeight = 20;

/* Control */

typedef enum {
  CONTROL_LABEL, 
  CONTROL_BUTTON, 
  CONTROL_TOGGLE, 
  CONTROL_CHOICE, 
  CONTROL_FIELD
} ControlType;

typedef struct {
  ControlType controlType;
  int panelId;
  AutoGL_Property *property;
  HWND window;
  int controlId;
  HWND itemWindows[AUTOGL_MAX_PROPERTY_ITEMS];
  int itemControlIds[AUTOGL_MAX_PROPERTY_ITEMS];
} Control;

#define MAX_CONTROLS 200
static int NControls;
static Control Controls[MAX_CONTROLS];

static int CurrentControlId = 1;

/* Panel */

static char *PanelClassName = "Panel";   /* Windows window class name */
static WNDCLASSEX PanelClass;            /* Windows window class */

#define MAX_PANELS 20
static int NPanels;
static AutoGL_Property *PanelPropertys[MAX_PANELS];
static HWND PanelWindows[MAX_PANELS];    /* Windows window handle */
static int PanelIsShown[MAX_PANELS];
static char PanelTitles[MAX_PANELS][AUTOGL_PROPERTY_LABEL_LENGTH];

/* View */

static char *ViewClassName = "View";   /* Windows window class name */
static WNDCLASSEX ViewClass;   /* Windows window class */
static HWND ViewWindow;        /* Windows window handle */
static HDC ViewDc;             /* Windows device context */
static HGLRC ViewRc;           /* OpenGL rendering context */

static AutoGL_CallbackType ViewRedrawCallback;
static AutoGL_CallbackType ViewEventCallback;
static int ViewIsDragging;

static int ViewUpdateIsForced;



/* Control */

static void Control_initialize 
(Control *control,
 int panelId,
 AutoGL_Property *property) 
{
  int iItem;

  switch (property->type) {
  case AUTOGL_PROPERTY_GROUP:
    assert(0);
    break;
  case AUTOGL_PROPERTY_COMMENT:
    control->controlType = CONTROL_LABEL;
    break;
  case AUTOGL_PROPERTY_CALLBACK:
    control->controlType = CONTROL_BUTTON;
    break;
  case AUTOGL_PROPERTY_BOOLEAN:
    control->controlType = CONTROL_TOGGLE;
    break;
  case AUTOGL_PROPERTY_STRING:
    control->controlType = CONTROL_FIELD;
    break;
  case AUTOGL_PROPERTY_INTEGER:
    if (0 < property->nItems) {
      control->controlType = CONTROL_CHOICE;
    } else {
      control->controlType = CONTROL_FIELD;
    }
    break;
  case AUTOGL_PROPERTY_REAL:
    control->controlType = CONTROL_FIELD;
    break;
  default:
    assert(0);
    break;
  }

  assert(0 <= panelId);
  control->panelId = panelId;
  control->property = property;
  control->window = NULL;
  control->controlId = -1;
  for (iItem = 0; iItem < property->nItems; iItem++) {
    control->itemWindows[iItem] = NULL;
    control->itemControlIds[iItem] = -1;
  }
}

static void Control_setWindow 
(Control *control,
 HWND window, int controlId) 
{ 
  assert (window != NULL);
  control->window = window;  
  assert (0 <= controlId);
  control->controlId = controlId;  
}

static void Control_setItemWindow 
(Control *control,
 int itemId, HWND window, int controlId) 
{ 
  assert (0 <= itemId);
  assert (itemId < control->property->nItems);
  assert (window != NULL);
  control->itemWindows[itemId] = window;  
  assert (0 <= controlId);
  control->itemControlIds[itemId] = controlId;  
}




/* Panel */

static void GetPanelSize 
(int *width_OUT, int *height_OUT,
 int panelId)
{
  int iControl; 

  *width_OUT = 0;
  *height_OUT = 0;

  for (iControl = 0; iControl < NControls; iControl++) {
    Control *control = &Controls[iControl];
    int width, height;
    char *controlLabel;
    int controlLabelLength;

    if (control->panelId != panelId) continue;
    
    width = 0;
    height = 0;
    controlLabel = AutoGL_INSIDE_GetPropertyLabel (control->property);
    controlLabelLength = strlen (controlLabel);

    switch (control->controlType) {
    case CONTROL_LABEL:
      {
	width = UnitWidth * (controlLabelLength + 2);
	height = UnitHeight;
      }
      break;
    case CONTROL_BUTTON:
      {
	width = UnitWidth * (controlLabelLength + 2);
	height = UnitHeight;
      }
      break;
    case CONTROL_TOGGLE:
      {
	width = UnitWidth * (controlLabelLength + 2);
	height = UnitHeight;
      }
      break;
    case CONTROL_CHOICE: 
      {
	int iItem; 

	width = UnitWidth * (controlLabelLength + 2 + 2);
	for (iItem = 0; iItem < control->property->nItems; iItem++) {
	  char *itemLabel = control->property->itemLabels[iItem];
	  int itemLabelLength = strlen (itemLabel);

	  if (width < UnitWidth * (itemLabelLength + 2 + 2)) {
	    width = UnitWidth * (itemLabelLength + 2 + 2);
	  }
	}
	height = UnitHeight * (control->property->nItems + 1);
      }
      break;
    case CONTROL_FIELD:
      {
	int editSize = 20;

	width = UnitWidth * (controlLabelLength + editSize);
	height = UnitHeight;
      }
      break;
    default:
      assert(0);
      break;
    }

    if (*width_OUT < width) {
      *width_OUT = width;
    }
    *height_OUT += height;
  }

  *width_OUT += 20;
  *height_OUT += 60;
}
  
static void RealizeContents (int panelId, HWND parentWindow)
{
  int originX = 10;
  int originY = 10;
  int iControl; 

  for (iControl = 0; iControl < NControls; iControl++) {
    Control *control = &Controls[iControl];
    char *controlLabel;
    int controlLabelLength;

    if (control->panelId != panelId) continue;

    controlLabel = AutoGL_INSIDE_GetPropertyLabel (control->property);
    controlLabelLength = strlen (controlLabel);

    switch (control->controlType) {
    case CONTROL_LABEL:
      {
	int width = UnitWidth * (controlLabelLength + 2);
	int height = UnitHeight;
	HWND window;

	/* create a static control */
	window = CreateWindow 
	  ("static",              /* static class */
	   controlLabel,          /* caption */
	   /* style : static */
	   WS_CHILD | WS_VISIBLE | SS_LEFT,
	   originX, originY,
	   width, height, 
	   parentWindow,          /* parent window */
	   (HMENU)CurrentControlId,      /* control ID */
	   TheInstance,  /* instance of this application program */
	   NULL   /* no additional arguments */
	   );
	CurrentControlId++;
	originY += height;
      }
      break;
    case CONTROL_BUTTON:
      {
	int width = UnitWidth * (controlLabelLength + 2);
	int height = UnitHeight;
	HWND window;

	/* create a push button control */
	window = CreateWindow 
	  ("button",              /* button class */
	   controlLabel,          /* caption */
	   /* style : push button */
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   originX, originY,
	   width, height, 
	   parentWindow,          /* parent window */
	   (HMENU)CurrentControlId,      /* control ID */
	   TheInstance,  /* instance of this application program */
	   NULL   /* no additional arguments */
	   );
	Control_setWindow 
	  (control, 
	   window, CurrentControlId);
	CurrentControlId++;
	originY += height;
      }
      break;
    case CONTROL_TOGGLE:
      {
	int width = UnitWidth * (controlLabelLength + 2);
	int height = UnitHeight;
	HWND window;
	int flag;
	
	/* create a check box control */
	window = CreateWindow 
	  ("button",              /* button class */
	   controlLabel,  /* caption */
	   /* style : check box */
	   WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
	   originX, originY,
	   width, height, 
	   parentWindow,          /* parent window */
	   (HMENU)CurrentControlId,      /* control ID */
	   TheInstance,  /* instance of this application program */
	   NULL   /* no additional arguments */
	   );
	Control_setWindow 
	  (control, 
	   window, CurrentControlId);
	CurrentControlId++;
	originY += height;
	
	flag = *control->property->booleanValue;

	/* set flag to the check box */
	SendMessage (window, BM_SETCHECK, (WPARAM)flag, 0);
      }
      break;
    case CONTROL_CHOICE: 
      {
	int width, height;
	int iItem; 
	int value;
	int itemOffsetX, itemOffsetY;
	HWND groupBoxWindow;

	width = UnitWidth * (controlLabelLength + 2 + 2);
	for (iItem = 0; iItem < control->property->nItems; iItem++) {
	  char *itemLabel = control->property->itemLabels[iItem];
	  int itemLabelLength = strlen (itemLabel);

	  if (width < UnitWidth * (itemLabelLength + 2 + 2)) {
	    width = UnitWidth * (itemLabelLength + 2 + 2);
	  }
	}
	height = UnitHeight * (control->property->nItems + 1);

	groupBoxWindow = CreateWindow 
	  ("button",               /* button class */
	   controlLabel,   /* caption */
	   /* style : group box */
	   WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	   originX, originY,
	   width, height, 
	   parentWindow,   /* parent window */
	   (HMENU)CurrentControlId,  /* control ID */
	   TheInstance,  /* instance of this application program */
	   NULL   /* no additional arguments */
	   );
	CurrentControlId++;
  
	value = *control->property->integerValue;

	itemOffsetX = UnitWidth;
	itemOffsetY = UnitHeight;
	for (iItem = 0; iItem < control->property->nItems; iItem++) {
	  char *itemLabel = control->property->itemLabels[iItem];
	  int itemLabelLength = strlen (itemLabel);
	  int itemLabelWidth = UnitWidth * (itemLabelLength + 2);
	  HWND window;

	  window = CreateWindow 
	    ("button",            /* button class */
	     itemLabel,         /* caption */
	     /* style : radio button */
	     WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
	     originX + itemOffsetX, 
	     originY + itemOffsetY,
	     itemLabelWidth, 
	     UnitHeight, 
	     parentWindow,   /* parent window */
	     (HMENU)CurrentControlId,  /* control ID */
	     TheInstance,  /* instance of this application program */
	     NULL   /* no additional arguments */
	     );
	  Control_setItemWindow 
	    (control, 
	     iItem, window, CurrentControlId);
	  CurrentControlId++;
	  itemOffsetY += UnitHeight;
	  
	  if (value == iItem) {
	    SendMessage (window, BM_SETCHECK, (WPARAM)1, 0);
	  } else {
	    SendMessage (window, BM_SETCHECK, (WPARAM)0, 0);
	  }
	}

	originY += height;
      }
      break;
    case CONTROL_FIELD:
      {
	int editSize = 20;
	int labelWidth = UnitWidth * controlLabelLength;
	int width = labelWidth + UnitWidth * editSize;
	int height = UnitHeight;
	HWND staticWindow;
	HWND editWindow;
	static char buf[256];

	/* create a static control */
	staticWindow = CreateWindow 
	  ("static",              /* static class */
	   controlLabel,  /* caption */
	   /* style : static */
	   WS_CHILD | WS_VISIBLE | SS_LEFT,
	   originX, 
	   originY,
	   labelWidth, 
	   height, 
	   parentWindow,          /* parent window */
	   (HMENU)CurrentControlId,      /* control ID */
	   TheInstance,  /* instance of this application program */
	   NULL   /* no additional arguments */
	   );
	CurrentControlId++;
	
	/* create an edit control */
	editWindow = CreateWindow 
	  ("edit",               /* edit class */
	   NULL,                 /* text */
	   /* style : edit */
	   WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
	   originX + labelWidth, 
	   originY,  
	   width - labelWidth, 
	   height,
	   parentWindow,          /* parent window */
	   (HMENU)CurrentControlId,      /* control ID */
	   TheInstance,  /* instance of this application program */
	   NULL   /* no additional arguments */
	   );
	Control_setWindow 
	  (control, 
	   editWindow, CurrentControlId);
	CurrentControlId++;
	originY += height;
	
	AutoGL_INSIDE_GetPropertyString (control->property, 
					 buf);
	
	/* set text to the edit control */
	SetWindowText (editWindow, buf);
      }
      break;
    default:
      assert(0);
      break;
    }
  }
}

static void ProcessControlEvent 
(int controlId, int code)
{
  int iControl;

  for (iControl = 0; iControl < NControls; iControl++) {
    Control *control = &Controls[iControl];

    if (control->controlId == controlId) {
      switch (control->controlType) {
      case CONTROL_BUTTON:
	{
	  AutoGL_INSIDE_InvokeCallback (control->property);
	}
        break;
      case CONTROL_TOGGLE:
	{
	  HWND window = control->window;
	  int flag = *control->property->booleanValue;

	  flag = !flag;
	  *control->property->booleanValue = flag;
	  
	  /* set status to the check box */
	  SendMessage (window, BM_SETCHECK, (WPARAM)flag, 0);
	}
        break;
      case CONTROL_FIELD:
	{
	  HWND window = control->window;
	  static char buf[256];
	  int length;

	  /* get text from the edit control */
	  length = GetWindowText (window, 
				  buf, 256 - 2);
	  if (0 < length) {
	    AutoGL_INSIDE_SetPropertyString (control->property, buf);
	  }
	}
        break;
      default:
	break;
      }
      
      break;
    }
  }

  for (iControl = 0; iControl < NControls; iControl++) {
    Control *control = &Controls[iControl];
    int newValue;
    int iItem; 

    if (control->controlType != CONTROL_CHOICE) {
      continue;
    }

    newValue = -1;
    for (iItem = 0; iItem < control->property->nItems; iItem++) {
      if (control->itemControlIds[iItem] == controlId) {
	newValue = iItem;
	break;
      }
    }
    
    if (0 <= newValue) {
      *control->property->integerValue = newValue;
      for (iItem = 0; iItem < control->property->nItems; iItem++) {
	HWND itemWindow = control->itemWindows[iItem];
	
	assert (itemWindow != NULL);
	if (iItem == newValue) {
	  SendMessage (itemWindow, BM_SETCHECK, (WPARAM)1, 0);
	} else {
	  SendMessage (itemWindow, BM_SETCHECK, (WPARAM)0, 0);
	}
      }
      break;
    }
  }

}

/* window procedure of panel */
static LRESULT CALLBACK PanelProcedure
(HWND window,   /* Windows window */
 UINT message,   /* Windows message ID */
 WPARAM wParam, LPARAM lParam   /* Windows message additional data */
 )
{
  static int currentCreatingPanelId = 0;
  int panelId;

  switch (message) {
  case WM_CREATE:  /* window is created */
    {
      /* set system fixed font into the device context */
      SelectObject (GetDC (window), 
		    GetStockObject (SYSTEM_FIXED_FONT));

      RealizeContents (currentCreatingPanelId, window);
      currentCreatingPanelId++;
    }
    break;
  case WM_CLOSE:  /* window is closed by Asakawa 2011 */ 
    for (panelId = 0; panelId < NPanels; panelId++) {
      if (window == PanelWindows[panelId]) {
        PanelIsShown[panelId] = 0;
        ShowWindow (PanelWindows[panelId], SW_HIDE);
      }
    }
    break;
  case WM_DESTROY:  /* window is destroyed */
    break;
  case WM_COMMAND:  /* control sends message */
    {
      int controlId = LOWORD(wParam);   /* control ID */
      int code = HIWORD(wParam);   /* notification code */

      ProcessControlEvent (controlId, code);
    }
    break;
  default:
    /* default behaviour by Windows */
    return DefWindowProc (window, message, wParam, lParam);
  }

  return 0;
}

/* register Windows window class of panel */
static int RegisterPanelClass (void)
{
  /* size of <PanelClass> */
  PanelClass.cbSize = sizeof (WNDCLASSEX);

  /* default window style */
  PanelClass.style = 0;

  /* set window procedure */
  PanelClass.lpfnWndProc 
    = (WNDPROC)PanelProcedure;

  PanelClass.cbClsExtra = 0;
  PanelClass.cbWndExtra = 0;

  /* set instance of this application program */
  PanelClass.hInstance = TheInstance;

  /* set default icons */
  PanelClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  PanelClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  
  /* set arrow cursor */
  PanelClass.hCursor = LoadCursor (NULL, IDC_ARROW);

  /* set dialog background color as background */
  PanelClass.hbrBackground 
    = (HBRUSH)(COLOR_BTNFACE + 1);

  /* set no menu */
  PanelClass.lpszMenuName = NULL;

  /* set window class name */
  PanelClass.lpszClassName = PanelClassName;

  /* register <PanelClass> */
  if (RegisterClassEx (&PanelClass) == 0) {
    fprintf (stderr, " failed to register Panel window class \n");
    return FALSE;
  }

  return TRUE;
}

static void RealizePanels (void)
{
  int iPanel; 

  /* create <PanelWindows> */
  for (iPanel = 0; iPanel < NPanels; iPanel++) {
    int width;
    int height;
    char *title;

    GetPanelSize (&width, &height,
		  iPanel);
    title = AutoGL_INSIDE_GetPropertyLabel (PanelPropertys[iPanel]);
    
    /* create panel window */
    PanelWindows[iPanel] = CreateWindow 
      (PanelClassName,          /* window class name */
       title,                   /* window title */
       WS_OVERLAPPEDWINDOW,     /* window style */
       /* x, y : use default */
       CW_USEDEFAULT, CW_USEDEFAULT,   
       width, height,
       NULL,           /* no parent window */
       NULL,           /* no menu */
       TheInstance,    /* instance of this application program */
       NULL            /* no additional arguments */
       );

    PanelIsShown[iPanel] = 0;
    strcpy (PanelTitles[iPanel], title);
  }
}

static void MakePanels (void)
{
  int nPropertys;
  int iProperty;

  nPropertys = AutoGL_INSIDE_GetNPropertys ();
  for (iProperty = 0; iProperty < nPropertys; iProperty++) {
    AutoGL_Property *property = AutoGL_INSIDE_GetProperty (iProperty);
    
    switch (property->type) {
    case AUTOGL_PROPERTY_GROUP: 
      {
	assert(NPanels < MAX_PANELS);
	PanelPropertys[NPanels] = property;
	NPanels++;
      }
      break;
    case AUTOGL_PROPERTY_COMMENT:
    case AUTOGL_PROPERTY_CALLBACK:
    case AUTOGL_PROPERTY_BOOLEAN:
    case AUTOGL_PROPERTY_STRING:
    case AUTOGL_PROPERTY_INTEGER:
    case AUTOGL_PROPERTY_REAL:
      {
	Control *control;
	
	assert (NControls < MAX_CONTROLS);
	control = &Controls[NControls];
	NControls++;
	
	Control_initialize 
	  (control, 
	   NPanels - 1, property);
      }
      break;
    default:
      assert(0);
      break;
    }
  }
}



/* View */

static void InitializeOpenGL (HWND window)
{
  /* description of pixel format */
  static PIXELFORMATDESCRIPTOR pfd = {
    sizeof (PIXELFORMATDESCRIPTOR),  /* size of <pfd> */
    1,  /* version of the structure */
    /* style : window, OpenGL, double buffer */
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,  /* color mode : RGBA */
    24,   /* color size : 24bit color */
    0, 0, 0, 0, 0, 0,   /* not used */
    0, 0,   /* not used */
    0, 0, 0, 0, 0,   /* not used */
    32,  /* Z buffer size : 32 bit */
    0,   /* not used */
    0,   /* not used */
    PFD_MAIN_PLANE,  /* draw in main plane */
    0,   /* not used */
    0, 0, 0   /* not used */
  };

  int pixelFormat;
  
  /* get device context */
  ViewDc = GetDC (window);
  
  /* set pixel formant of OpenGL window */
  
  /* find appropriate pixel format */
  pixelFormat = ChoosePixelFormat (ViewDc, &pfd);
  
  /* set <pixelFormat> */
  SetPixelFormat (ViewDc, pixelFormat, &pfd);
  
  /* create OpenGL rendering context from device context */
  ViewRc = wglCreateContext (ViewDc);
  
  /* set rendering context as current */
  wglMakeCurrent (ViewDc, ViewRc);
}

static void RealizeViewContents (void)
{
  AutoGL_INSIDE_RealizeOpenGLWindow ();

  /* set system fixed font */
  SelectObject (ViewDc, GetStockObject (SYSTEM_FIXED_FONT));
  
  /* prepare bitmaps for OpenGL character string drawing */
  wglUseFontBitmaps (ViewDc, 0, 128, AutoGL_INSIDE_GetOpenGLCharBase ());
}

static void TerminateOpenGL (void)
{
  /* release rendering context */
  wglMakeCurrent (ViewDc, NULL);
  
  /* delete rendering context */
  wglDeleteContext (ViewRc);
}

static void RedrawView (void)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  if (!AutoGL_INSIDE_IsSynchronized ()) {
    ViewUpdateIsForced = 0;
    return;
  }
#endif

  if (AutoGL_INSIDE_IsMaster ()
      || ViewUpdateIsForced) {
    AutoGL_INSIDE_StartDrawingOpenGLWindow ();
    AutoGL_INSIDE_InvokeViewRedrawCallback ();
    AutoGL_INSIDE_EndDrawingOpenGLWindow ();
    
    /* switch front and back buffers and show rendered image */
    SwapBuffers (ViewDc);
  }

  ViewUpdateIsForced = 0;

  /* validate the redrawn client area */
  ValidateRect (ViewWindow, NULL);
}

static void ProcessViewControlEvent 
(int controlId, int code)
{
  int panelId = controlId - 1;

  assert(0 <= panelId);
  assert(panelId < NPanels);

  if (PanelIsShown[panelId]) {
    PanelIsShown[panelId] = 0;
    ShowWindow (PanelWindows[panelId], SW_HIDE);
  } else {
    PanelIsShown[panelId] = 1;
    ShowWindow (PanelWindows[panelId], SW_SHOW);
    UpdateWindow (PanelWindows[panelId]);
  }
}

static void GetDcOfMouse 
(int *dcX_OUT, int *dcY_OUT, int *dcZ_OUT,
 int mouseX, int mouseY)
{
  int widthDc, heightDc;

  AutoGL_GetViewRangeDc (&widthDc, &heightDc);
  *dcX_OUT = mouseX - widthDc / 2;
  *dcY_OUT = heightDc / 2 - mouseY;
  *dcZ_OUT = 0;
}

/* window procedure of View window */
static LRESULT CALLBACK ViewProcedure
(HWND window,   /* Windows window */
 UINT message,  /* Windows message ID */
 WPARAM wParam, LPARAM lParam   /* Windows message additional data */
 )
{
  switch (message) {
  case WM_CREATE:  /* window is created */
    {
      InitializeOpenGL (window);
      RealizeViewContents ();
    }
    break;
  case WM_DESTROY:  /* window is destroyed */
    {
      TerminateOpenGL ();
      PostQuitMessage(0);   /* by Asakawa 2011 */ 
    }
    break;
  case WM_SIZE:  /* window size is changed */
    {
      int width = LOWORD (lParam);
      int height = HIWORD (lParam);

      AutoGL_INSIDE_SetViewRangeDc (width, height);
    }
    break;
  case WM_PAINT:  /* window is redrawn */
    {
      RedrawView ();
    }
    break;
  case WM_COMMAND:  /* control sends message */
    {
      int controlId = LOWORD(wParam);   /* control ID */
      int code = HIWORD(wParam);   /* notification code */

      ProcessViewControlEvent (controlId, code);
    }
    break;
  case WM_LBUTTONDOWN:  /* mouse button down */
    {
      int x = LOWORD (lParam);  /* mouse x position */
      int y = HIWORD (lParam);  /* mouse y position */
      int dcX, dcY, dcZ;

      GetDcOfMouse (&dcX, &dcY, &dcZ,
		    x, y);
      AutoGL_INSIDE_SetPointingDeviceEvent 
	(AUTOGL_EVENT_POINTING_DEVICE_PRESS, 
	 dcX, dcY, dcZ);
      AutoGL_INSIDE_InvokeViewEventCallback ();

      ViewIsDragging = 1;
    }
    break;
  case WM_MOUSEMOVE:  /* mouse move */
    if (ViewIsDragging) {
      int x = LOWORD (lParam);  /* mouse x position */
      int y = HIWORD (lParam);  /* mouse y position */
      int dcX, dcY, dcZ;

      GetDcOfMouse (&dcX, &dcY, &dcZ,
		    x, y);
      AutoGL_INSIDE_SetPointingDeviceEvent 
	(AUTOGL_EVENT_POINTING_DEVICE_DRAG, 
	 dcX, dcY, dcZ);
      AutoGL_INSIDE_InvokeViewEventCallback ();
    }
    break;
  case WM_LBUTTONUP:  /* mouse button up */
    {
      int x = LOWORD (lParam);  /* mouse x position */
      int y = HIWORD (lParam);  /* mouse y position */
      int dcX, dcY, dcZ;

      GetDcOfMouse (&dcX, &dcY, &dcZ,
		    x, y);
      AutoGL_INSIDE_SetPointingDeviceEvent 
	(AUTOGL_EVENT_POINTING_DEVICE_RELEASE, 
	 dcX, dcY, dcZ);
      AutoGL_INSIDE_InvokeViewEventCallback ();

      ViewIsDragging = 0;
    }
    break;
  case WM_CHAR:  /* keyboard event */
    {
      AutoGL_INSIDE_SetKeyEvent (wParam);
      AutoGL_INSIDE_InvokeViewEventCallback ();
    }
    break;
  case WM_TIMER:  /* timer (idle) event */
    {
      AutoGL_INSIDE_InvokeIdleEventCallback ();
    }
    break;
  default:
    /* default behaviour by Windows */
    return DefWindowProc (window, message, wParam, lParam);
  }

  return 0;
}

/* register Windows window class of View window */
static int RegisterViewClass (void)
{
  /* size of <ViewClass> */
  ViewClass.cbSize = sizeof (WNDCLASSEX);
  
  /* redraw everytime if either horizontal or vertical size of 
     the window changed */
  ViewClass.style = CS_HREDRAW | CS_VREDRAW;

  /* set window procedure */
  ViewClass.lpfnWndProc 
    = (WNDPROC)ViewProcedure;

  ViewClass.cbClsExtra = 0;
  ViewClass.cbWndExtra = 0;

  /* set instance of this application program */
  ViewClass.hInstance = TheInstance;

  /* set default icons */
  ViewClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  ViewClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  
  /* set arrow cursor */
  ViewClass.hCursor = LoadCursor (NULL, IDC_ARROW);

  /* set no background : necessary for OpenGL window */
  ViewClass.hbrBackground = NULL;

  /* set no menu */
  ViewClass.lpszMenuName = NULL;

  /* set window class name */
  ViewClass.lpszClassName = ViewClassName;

  /* register <ViewClass> */
  if (RegisterClassEx (&ViewClass) == 0) {
    fprintf (stderr, " failed to register View class \n");
    return FALSE;
  }

  return TRUE;
}

static void CreateView (void)
{
  char *label;
  HMENU menuBar;
  HMENU panelMenu;
  int iPanel;
  int widthDc, heightDc;

  if (AutoGL_INSIDE_IsMaster ()) {
    label = "View";
  } else {
    label = "View (slave)";
  }

  /* make menu bar */
  menuBar = CreateMenu ();

  if (AutoGL_INSIDE_IsMaster ()) {

    /* make panel menu */
    panelMenu = CreateMenu ();
    for (iPanel = 0; iPanel < NPanels; iPanel++) {
      int controlId = iPanel + 1;

      AppendMenu (panelMenu, MF_STRING, controlId, PanelTitles[iPanel]);
    }
    AppendMenu (menuBar, MF_POPUP, (UINT)panelMenu, "Panel");

  }

  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);

  /* create <ViewWindow> */
  ViewWindow = CreateWindow 
    (ViewClassName,         /* window class name */
     label,                 /* window title */
     /* window style : necessary for OpenGL window */
     WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
     /* x, y : use default */
     CW_USEDEFAULT, CW_USEDEFAULT,     
     widthDc, heightDc, 
     NULL,           /* no parent window */
     menuBar,
     TheInstance,    /* instance of this application program */
     NULL            /* no additional arguments */
     );
  assert(ViewWindow != NULL);
}

static void RealizeView (void)
{
  CreateView ();
  
  /* show <ViewWindow> on the screen */
  ShowWindow (ViewWindow, SW_SHOW);
  UpdateWindow (ViewWindow);
  
  /* idle event (id == 1) for each 1 msec */
  SetTimer (ViewWindow, 1, 1, NULL);
  
  /* NEED TO REDEFINE */
  /* synchronize after several seconds in AutoGL-MP */
}
  
static void MakeView (void)
{
  ViewRedrawCallback = AutoGL_INSIDE_GetViewRedrawCallback ();
  ViewEventCallback = AutoGL_INSIDE_GetViewEventCallback ();
}



void AutoGL_INSIDE_ResizeView (int widthDc, int heightDc)
{
  AutoGL_INSIDE_SetViewRangeDc (widthDc, heightDc);

  /* NEED TO REDEFINE */
  /* how to resize view window ??? */
}

void AutoGL_INSIDE_UpdateView (void)
{
  ViewUpdateIsForced = 1;
  RedrawView ();
}



void AutoGL_ResizeView (int widthDc, int heightDc)
{
  AutoGL_INSIDE_SetViewRangeDc (widthDc, heightDc);

  if (!AutoGL_INSIDE_IsMaster ()) return;

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  AutoGL_INSIDE_SendResizeEvent (widthDc, heightDc);
#else
  AutoGL_INSIDE_ResizeView (widthDc, heightDc);
#endif
}

void AutoGL_DrawView (void)
{
  if (!AutoGL_INSIDE_IsMaster ()) return;

  AutoGL_INSIDE_UpdateView ();
}

void AutoGL_Terminate (void)
{
  if (!AutoGL_INSIDE_IsMaster ()) return;

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  AutoGL_INSIDE_SendQuitEvent ();
#else
  /* do nothing */
#endif
}



/* Command Line Argument */

#define MAX_ARGUMENTS 100
#define ARGUMENT_LENGTH 80
static int NArguments;
static char Arguments[MAX_ARGUMENTS][ARGUMENT_LENGTH];
static char *Argv[MAX_ARGUMENTS + 1];

static void SetArguments (const char *commandLine)
{
  int iSrc, iDest;
  int iArgument;
  
  /* NEED TO REDEFINE */
  /* how to get command name ? */
  strcpy (Arguments[0], "???");
  NArguments = 1;

  iSrc = 0;
  while (1) {
    while (commandLine[iSrc] == ' ') {
      iSrc++;
    }
    if (commandLine[iSrc] == 0) {
      break;
    }

    assert(NArguments < MAX_ARGUMENTS);
    iDest = 0;
    while (commandLine[iSrc] != 0
	   && commandLine[iSrc] != ' ') {
      assert(iDest < ARGUMENT_LENGTH - 1);
      Arguments[NArguments][iDest] = commandLine[iSrc];
      iSrc++;
      iDest++;
    }
    Arguments[NArguments][iDest] = 0;
    NArguments++;
  }

  for (iArgument = 0; iArgument < NArguments; iArgument++) {
    Argv[iArgument] = Arguments[iArgument];
  }
  Argv[NArguments] = NULL;
}






/* Main Routine */

static MSG Message;    /* Windows message */

static int Initialize (HINSTANCE instance)
{
  TheInstance = instance;
  
  if (RegisterPanelClass () == FALSE) {
    return 0;
  }
  if (RegisterViewClass () == FALSE) {
    return 0;
  }
  
  if (AutoGL_INSIDE_IdleEventIsEnabled ()) {
    /* NEED TO REDEFINE */
    /* set timer to handle idle event */
    /* synchronize for AutoGL-MP */
  }
  
  if (AutoGL_INSIDE_IsMaster ()) {
    MakePanels ();
  }
  MakeView ();

  if (AutoGL_INSIDE_IsMaster ()) {
    RealizePanels ();
  }
  RealizeView ();

  return 1;
}

void AutoGL_MainLoop (void) 
{
  /* process Windows message loop */
  while (GetMessage (&Message, NULL, 0, 0)) {
    TranslateMessage (&Message);
    DispatchMessage (&Message);
  }
}

#ifdef AUTOGL_FORTRAN

void AutoGL_Main (void) 
{
  int flag;

  /* NEED TO REDEFINE */
  /* no argc, argv */
  /* how to initialize AutoNOSim ??? */

  flag = Initialize (0);
  if (!flag) {
    return;
  }

  AutoGL_MainLoop ();
}

#else

/* WinMain function : entry point of Windows application */
int APIENTRY WinMain 
(HINSTANCE instance,   /* application instance */
 HINSTANCE prevInstance, /* not relevant */
 LPSTR commandLine,   /* command line arguments */
 int windowStatus   /* window status such as open, close, icon */
 )
{
  int flag;

  SetArguments (commandLine);

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  /* NEED TO REDEFINE */
  /* does it work ??? */
  /* off course, not... */
  AutoGL_INSIDE_InitializeAutoNOSim (&NArguments, &Argv);
#endif

  AutoGL_SetUp (NArguments, Argv);
  
  flag = Initialize (instance);
  if (!flag) {
    return FALSE;
  }

  AutoGL_MainLoop ();
  
  /* the end of the process */
  return Message.wParam;
}

#endif


