/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_gui_motif.c : OSF/Motif and OpenGL */

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

#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include <Xm/XmAll.h>

#define noMotifGLwidget
#define GLW_HDR_ANOTHER_LOCATION 

#ifdef noMotifGLwidget
#ifdef GLW_HDR_ANOTHER_LOCATION 
#include <GL/GLwDrawA.h>
#else
#include <X11/GLw/GLwDrawA.h>
#endif
#else
#ifdef GLW_HDR_ANOTHER_LOCATION 
#include <GL/GLwMDrawA.h>
#else
#include <X11/GLw/GLwMDrawA.h>
#endif
#endif

/* usleep */
#include <unistd.h>



/* Module Variables */

static int Buffer[] = {
  GLX_DOUBLEBUFFER, GLX_RGBA, GLX_DEPTH_SIZE, 16,
  GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,
  None
};

static Display *MyDisplay;
static XtAppContext MyApplicationContext;
static XVisualInfo *MyVisualInformation;
static GLXContext MyGlxContext;
static Widget TopLevelWindow;
static Widget MainWindow;
static Widget MenuBar;
static Widget PanelMenu;
static Widget PanelCascade;
static Widget ViewWindow;

static Widget CurrentPanelWindow;
static Widget CurrentPanel;

#define MAX_PANELS 20
static int NPanels;
static Widget PanelWindows[MAX_PANELS];
static int PanelIds[MAX_PANELS];
static int PanelIsShown[MAX_PANELS];
static int PanelIsRealized[MAX_PANELS];
static char PanelTitles[MAX_PANELS][AUTOGL_PROPERTY_LABEL_LENGTH];

static int ViewUpdateIsForced;



/* Panel */

static void MakePanel (AutoGL_Property *property) 
{
  char *title;

  title = AutoGL_INSIDE_GetPropertyLabel (property);

  CurrentPanelWindow = XtAppCreateShell 
    (title, title, topLevelShellWidgetClass, MyDisplay, NULL, 0);

  CurrentPanel = XtCreateManagedWidget 
    ("panel", xmRowColumnWidgetClass, CurrentPanelWindow, NULL, 0);
  XtManageChild (CurrentPanel);

  assert(NPanels < MAX_PANELS);
  PanelWindows[NPanels] = CurrentPanelWindow;
  PanelIds[NPanels] = NPanels;
  PanelIsShown[NPanels] = 0;
  PanelIsRealized[NPanels] = 0;
  strcpy (PanelTitles[NPanels], title);
  NPanels++;
}



/* Label */

static void AddLabel (AutoGL_Property *property) 
{
  Widget label;

  label = XtCreateManagedWidget 
    (AutoGL_INSIDE_GetPropertyLabel (property), 
     xmLabelWidgetClass, CurrentPanel, NULL, 0);
  XtManageChild (label);
}



/* Button */

static void ButtonCallback 
(Widget button, XtPointer clientData, XtPointer callData)
{
  AutoGL_Property *property = (AutoGL_Property *)clientData;

  AutoGL_INSIDE_InvokeCallback (property);
}

static void AddButton (AutoGL_Property *property) 
{
  Widget button;

  button = XtCreateManagedWidget 
    (AutoGL_INSIDE_GetPropertyLabel (property), 
     xmPushButtonWidgetClass, CurrentPanel, NULL, 0);
  XtManageChild (button);

  XtAddCallback (button, XmNactivateCallback, 
		 ButtonCallback, (XtPointer)property);
}



/* Toggle */

static void ToggleCallback 
(Widget toggle, XtPointer clientData, XtPointer callData)
{
  AutoGL_Property *property = (AutoGL_Property *)clientData;
  
  *property->booleanValue = XmToggleButtonGetState (toggle);
}

static void ToggleEventCallback  
(Widget toggle, XtPointer clientData, XEvent *event, Boolean *dispatch)
{
  AutoGL_Property *property = (AutoGL_Property *)clientData;

  if (event->type == Expose) {
    XmToggleButtonSetState (toggle, *property->booleanValue, FALSE);
  }
}

static void AddToggle (AutoGL_Property *property) 
{
  Widget toggle;

  toggle = XtCreateManagedWidget 
    (AutoGL_INSIDE_GetPropertyLabel (property), 
     xmToggleButtonWidgetClass, CurrentPanel, NULL, 0);
  XtManageChild (toggle);

  XtAddCallback (toggle, XmNvalueChangedCallback, 
		 ToggleCallback, (XtPointer)property);
  XtAddEventHandler 
    (toggle, 
     ExposureMask, 
     False, ToggleEventCallback, (XtPointer)property);

  XmToggleButtonSetState (toggle, *property->booleanValue, FALSE);
}



/* Field */

static void FieldCallback 
(Widget textField, XtPointer clientData, XtPointer callData)
{
  AutoGL_Property *property = (AutoGL_Property *)clientData;

  AutoGL_INSIDE_SetPropertyString (property, XmTextGetString (textField));
}

static void FieldEventCallback  
(Widget textField, XtPointer clientData, XEvent *event, Boolean *dispatch)
{
  AutoGL_Property *property = (AutoGL_Property *)clientData;
  static char buf[256];

  if (event->type == Expose) {
    AutoGL_INSIDE_GetPropertyString (property, 
				     buf);
    XmTextFieldSetString (textField, buf);
  }
}

static void AddField (AutoGL_Property *property) 
{
  Widget rowColumn;
  Widget label;
  Widget textField;
  static char buf[256];

  rowColumn = XtCreateManagedWidget 
    ("field", xmRowColumnWidgetClass, CurrentPanel, NULL, 0);
  XtManageChild (rowColumn);
  XtVaSetValues (rowColumn,
		 XmNorientation, XmHORIZONTAL,
		 NULL);

  label = XtCreateManagedWidget 
    (AutoGL_INSIDE_GetPropertyLabel (property), 
     xmLabelWidgetClass, rowColumn, NULL, 0);
  XtManageChild (label);

  textField = XmCreateTextField 
    (rowColumn, AutoGL_INSIDE_GetPropertyLabel (property), NULL, 0);
  XtManageChild (textField);

  XtAddCallback (textField, XmNvalueChangedCallback, 
		 FieldCallback, (XtPointer)property);
  XtAddEventHandler 
    (textField, 
     ExposureMask, 
     False, FieldEventCallback, (XtPointer)property);

  AutoGL_INSIDE_GetPropertyString (property, 
				   buf);
  XmTextFieldSetString (textField, buf);
}



/* Choice */

static void ChoiceCallback 
(Widget toggle, XtPointer clientData, XtPointer callData)
{
  XmToggleButtonCallbackStruct *cbs 
    = (XmToggleButtonCallbackStruct *)callData;
  AutoGL_Property *property = (AutoGL_Property *)clientData;
  int iItem;

  if (!cbs->set) return;

  for (iItem = 0; iItem < property->nItems; iItem++) {
    if (strcmp (XtName (toggle), 
		property->itemLabels[iItem]) == 0) {
      *property->integerValue = iItem;
      return;
    }
  }
}

/* NEED TO REDEFINE */
/* update choice at expose event */

static void AddChoice (AutoGL_Property *property) 
{
  Widget form;
  Widget frame;
  Widget label;
  Widget rowColumn;
  int iItem;

  form = XtCreateManagedWidget 
    ("form", xmFormWidgetClass, CurrentPanel, NULL, 0);

  frame = XtCreateManagedWidget 
    ("frame", xmFrameWidgetClass, form, NULL, 0);

  label = XtVaCreateManagedWidget 
    (AutoGL_INSIDE_GetPropertyLabel (property), 
     xmLabelWidgetClass, frame, 
     XmNchildType, XmFRAME_TITLE_CHILD, 
     NULL, 0);

  rowColumn = XmCreateRadioBox (frame, "rowColumn", NULL, 0);
  XtManageChild (rowColumn);

  for (iItem = 0; iItem < property->nItems; iItem++) {
    Widget toggle;

    toggle = XtCreateManagedWidget 
      (property->itemLabels[iItem], 
       xmToggleButtonWidgetClass, rowColumn, NULL, 0);
    XtAddCallback (toggle, XmNvalueChangedCallback, 
		   ChoiceCallback, (XtPointer)property);
    XtManageChild (label);

    if (*property->integerValue == iItem) {
      XmToggleButtonSetState (toggle, 1, FALSE);
    } else {
      XmToggleButtonSetState (toggle, 0, FALSE);
    }
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
      MakePanel (property);
      break;
    case AUTOGL_PROPERTY_COMMENT:
      AddLabel (property);
      break;
    case AUTOGL_PROPERTY_CALLBACK:
      AddButton (property);
      break;
    case AUTOGL_PROPERTY_BOOLEAN:
      AddToggle (property);
      break;
    case AUTOGL_PROPERTY_STRING:
    case AUTOGL_PROPERTY_INTEGER:
    case AUTOGL_PROPERTY_REAL:
      if (0 < property->nItems) {
	AddChoice (property);
      } else {
	AddField (property);
      }
      break;
    default:
      assert(0);
      break;
    }
  }
}



/* View */

static void ResizeView (void)
{
  Dimension width, height;

  glXMakeCurrent (MyDisplay, XtWindow (ViewWindow), MyGlxContext);

  XtVaGetValues (ViewWindow, XmNwidth, &width, XmNheight, &height, NULL);
  AutoGL_INSIDE_SetViewRangeDc (width, height);
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
    glXMakeCurrent (MyDisplay, XtWindow (ViewWindow), MyGlxContext);

    AutoGL_INSIDE_StartDrawingOpenGLWindow ();
    AutoGL_INSIDE_InvokeViewRedrawCallback ();
    AutoGL_INSIDE_EndDrawingOpenGLWindow ();
    glXSwapBuffers (MyDisplay, XtWindow (ViewWindow));
  }

  ViewUpdateIsForced = 0;
}

static void InitializeViewWindow (void)
{
  XFontStruct *font;

  glXMakeCurrent (MyDisplay, XtWindow (ViewWindow), MyGlxContext);

  AutoGL_INSIDE_RealizeOpenGLWindow ();

  /* make display list of character font */
  font = XLoadQueryFont (MyDisplay, "a14");
  glXUseXFont (font->fid, 32, 96, AutoGL_INSIDE_GetOpenGLCharBase () + 32);

  ResizeView ();
  RedrawView ();
}

static void ResizeViewCallback 
(Widget w, XtPointer data, XtPointer callData)
{
  ResizeView ();
}

static void RedrawViewCallback 
(Widget w, XtPointer data, XtPointer callData)
{
  RedrawView ();
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

static void ViewEventCallback 
(Widget w, XtPointer clientData, XEvent *event, Boolean *dispatch)
{
  int dcX, dcY, dcZ;

  if (event->type == ButtonPress 
      && event->xbutton.button == Button1) {
    GetDcOfMouse (&dcX, &dcY, &dcZ,
		  (int)event->xbutton.x, (int)event->xbutton.y);
    AutoGL_INSIDE_SetPointingDeviceEvent 
      (AUTOGL_EVENT_POINTING_DEVICE_PRESS, 
       dcX, dcY, dcZ);
    AutoGL_INSIDE_InvokeViewEventCallback ();

  } else if (event->type == ButtonRelease 
	     && event->xbutton.button == Button1) {
    GetDcOfMouse (&dcX, &dcY, &dcZ,
		  (int)event->xbutton.x, (int)event->xbutton.y);
    AutoGL_INSIDE_SetPointingDeviceEvent 
      (AUTOGL_EVENT_POINTING_DEVICE_RELEASE, 
       dcX, dcY, dcZ);
    AutoGL_INSIDE_InvokeViewEventCallback ();

  } else if (event->type == MotionNotify 
	     && event->xmotion.state & Button1Mask
	     && !(event->xmotion.state & ShiftMask)) {
    GetDcOfMouse (&dcX, &dcY, &dcZ,
		  (int)event->xbutton.x, (int)event->xbutton.y);
    AutoGL_INSIDE_SetPointingDeviceEvent 
      (AUTOGL_EVENT_POINTING_DEVICE_DRAG, 
       dcX, dcY, dcZ);
    AutoGL_INSIDE_InvokeViewEventCallback ();

  } else if (event->type == KeyRelease) {
    int keyChar;
    KeySym key;
    char string[10];

    if (XLookupString ((XKeyEvent*)event, 
		       string, 10, &key, 0) != 1) {
      return;
    }
    
    switch (key) {
    case XK_Return:
      keyChar = '\n';
      break;
    case XK_BackSpace:
      keyChar = '\b';
      break;
    case XK_Shift_R:
    case XK_Shift_L:
    case XK_Control_R:
    case XK_Control_L:
    case XK_Caps_Lock:
    case XK_Shift_Lock:
      return;
      break;
    default:
      if (!IsCursorKey (key) && !IsFunctionKey (key)) {
	keyChar = string[0];
      } else {
	return;
      }
      break;
    }
    
    AutoGL_INSIDE_SetKeyEvent (keyChar);
    AutoGL_INSIDE_InvokeViewEventCallback ();
  }
}

static void PanelCallback 
(Widget widget, XtPointer clientData, XtPointer callData)
{
  int panelId = *(int *)(clientData);
  
  /* WARNING */
  /* using Xlib : XMapWindow, XUnmapWindow */

  if (PanelIsShown[panelId]) {
    PanelIsShown[panelId] = 0;
    
    XUnmapWindow (MyDisplay, XtWindow (PanelWindows[panelId]));
    XFlush (MyDisplay);
  } else {
    PanelIsShown[panelId] = 1;
    
    if (!PanelIsRealized[panelId]) {
      PanelIsRealized[panelId] = 1;

      XtRealizeWidget (PanelWindows[panelId]);
    } else {
      XMapWindow (MyDisplay, XtWindow (PanelWindows[panelId]));
      XFlush (MyDisplay);
    }
  }
}

static void MakeView (void)
{
  int iPanel;
  int widthDc, heightDc;

  if (AutoGL_INSIDE_IsMaster ()) {
    XtVaSetValues (TopLevelWindow, 
		   XmNtitle, "View",
		   NULL);
  } else {
    XtVaSetValues (TopLevelWindow, 
		   XmNtitle, "View (slave)",
		   NULL);
  }

  MainWindow = XtCreateManagedWidget 
    ("mainwindow", xmMainWindowWidgetClass,
     TopLevelWindow, NULL, 0);		      



  /* make menu bar */
    
  MenuBar = XmCreateMenuBar (MainWindow, "menu", NULL, 0);

  XtManageChild (MenuBar);



  /* make panel menu */
    
  PanelMenu = XmCreatePulldownMenu (MenuBar, "Panel", NULL, 0);

  PanelCascade = XtVaCreateManagedWidget 
    ("Panel", xmCascadeButtonWidgetClass,
     MenuBar, 
     XmNsubMenuId, PanelMenu,
     NULL);
  
  if (AutoGL_INSIDE_IsMaster ()) {
    for (iPanel = 0; iPanel < NPanels; iPanel++) {
      Widget menuItem;
      
      menuItem = XtCreateManagedWidget 
	(PanelTitles[iPanel], xmPushButtonWidgetClass,
	 PanelMenu, NULL, 0);
      XtAddCallback (menuItem, XmNactivateCallback,
		     PanelCallback, 
		     (XtPointer)&PanelIds[iPanel]);
    }
  }



  /* make OpenGL drawing area using GLXArea widget */

#ifdef noMotifGLwidget
  ViewWindow = XtVaCreateManagedWidget
    ("glxarea", glwDrawingAreaWidgetClass,
     MainWindow, GLwNvisualInfo, MyVisualInformation, NULL);
#else
  ViewWindow = XtVaCreateManagedWidget
    ("glxarea", glwMDrawingAreaWidgetClass,
     MainWindow, GLwNvisualInfo, MyVisualInformation, NULL);
#endif

  XtAddCallback
    (ViewWindow, XmNexposeCallback, RedrawViewCallback, NULL);
  XtAddCallback 
    (ViewWindow, XmNresizeCallback, ResizeViewCallback, NULL);
  
  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);
  XtVaSetValues (ViewWindow, 
		 XmNwidth, widthDc, 
		 XmNheight, heightDc,
		 NULL);

  if (AutoGL_INSIDE_IsMaster ()) {
    XtAddEventHandler 
      (ViewWindow, 
       ButtonPressMask | ButtonReleaseMask | ButtonMotionMask 
       | KeyReleaseMask, 
       False, ViewEventCallback, NULL);
  }



  XtVaSetValues (MainWindow,
		 XmNmenuBar, MenuBar,
		 XmNworkWindow, ViewWindow,
		 NULL);

  XtRealizeWidget (TopLevelWindow);
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




/* Timer and Idle Event Handling */

/* NEED TO REDEFINE */
/* synchronize after several seconds in AutoGL-MP */

static Boolean WorkProcedure (XtPointer clientData)
{
  AutoGL_INSIDE_InvokeIdleEventCallback ();

  /* call each 10 msec */
  usleep (10000);

  return FALSE;
}

static void SetUpTimer (void)
{
  if (AutoGL_INSIDE_IsMaster ()) {
    if (AutoGL_INSIDE_IdleEventIsEnabled ()) {
      XtAppAddWorkProc (MyApplicationContext,
			WorkProcedure, (XtPointer)NULL);
    }
  }
}






/* Main Routine */

static void Initialize (int argc, char *argv[])
{
  TopLevelWindow = XtAppInitialize 
    (&MyApplicationContext, "autogl", NULL, 0, &argc, argv,
     NULL, NULL, 0 );

  MyDisplay = XtDisplay (TopLevelWindow);
  assert (MyDisplay != NULL);

  MyVisualInformation = glXChooseVisual
    (MyDisplay, DefaultScreen (MyDisplay), Buffer);
  assert (MyVisualInformation != NULL);

  MyGlxContext = glXCreateContext 
    (MyDisplay, MyVisualInformation, None, GL_TRUE);
  assert (MyGlxContext != NULL);

  if (AutoGL_INSIDE_IsMaster ()) {
    MakePanels ();
  }
  MakeView ();
  
  InitializeViewWindow ();
  SetUpTimer ();
}

void AutoGL_MainLoop (void) 
{
  XtAppMainLoop (MyApplicationContext);
}

#ifdef AUTOGL_FORTRAN

void AutoGL_Main (void) 
{
  /* NEED TO REDEFINE */
  /* no argc, argv */
  /* how to initialize AutoNOSim ??? */

  Initialize (0, NULL);

  AutoGL_MainLoop ();
}

#else

int main (int argc, char *argv[]) 
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  AutoGL_INSIDE_InitializeAutoNOSim (&argc, &argv);
#endif

  /* call the user-side set-up routine */
  AutoGL_SetUp (argc, argv);

  Initialize (argc, argv);

  AutoGL_MainLoop ();

  /* never return */
  return 0;
}

#endif
