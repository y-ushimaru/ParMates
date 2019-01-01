/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_gui_gtk.c : GTK+ and OpenGL */

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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <GL/glx.h>



/* Module Variables */

static GtkWidget *CurrentPanelWindow;
static GtkWidget *CurrentPanel;

static GtkWidget *ViewWindow;
static GtkWidget *ViewVBox;
static GtkWidget *ViewMenuBar;
static GtkWidget *ViewPanelMenuItem;
static GtkWidget *ViewPanelMenu;
static GtkWidget *TopGlArea;

typedef struct {
  GSList *slist;
  int *integerValue;
} ChoiceData;

#define MAX_CHOICES 1000
static int NChoices;
ChoiceData Choices[MAX_CHOICES];

#define MAX_PANELS 20
static int NPanels;
static GtkWidget *PanelWindows[MAX_PANELS];
static int PanelIds[MAX_PANELS];
static int PanelIsShown[MAX_PANELS];
static char PanelTitles[MAX_PANELS][AUTOGL_PROPERTY_LABEL_LENGTH];
static int ViewUpdateIsForced;

/* using X Window System (Xlib) */
static Display *AutoGL_XDisplay;

/* using X Window System extension for Open GL (GLX) */
static GLXContext AutoGL_GLXcontext;



/* Panel */

gboolean PanelDeleteCallback(GtkWidget *widget, GdkEvent *event, gpointer data){
                                          /* by Asakawa 2011 */ 
  int panelId;
  for (panelId = 0; panelId < NPanels; panelId++) {
    if (widget == PanelWindows[panelId]) {
      PanelIsShown[panelId] = 0;
      gtk_widget_hide (PanelWindows[panelId]);
    }
  }
  return TRUE;  /* not close */
}

static void MakePanel (AutoGL_Property *property) 
{
  char *title;

  title = AutoGL_INSIDE_GetPropertyLabel (property);
  
  CurrentPanelWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(CurrentPanelWindow), 
			title);

  gtk_signal_connect (GTK_OBJECT (CurrentPanelWindow), "delete_event",
              GTK_SIGNAL_FUNC (PanelDeleteCallback), NULL);  /* by Asakawa 2011 */ 

  CurrentPanel = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(CurrentPanelWindow), CurrentPanel);
  gtk_widget_show (CurrentPanel);
  
  assert(NPanels < MAX_PANELS);
  PanelWindows[NPanels] = CurrentPanelWindow;
  PanelIds[NPanels] = NPanels;
  PanelIsShown[NPanels] = 0;
  strcpy (PanelTitles[NPanels], title);
  NPanels++;
}



/* Label */

static void AddLabel (AutoGL_Property *property) 
{
  GtkWidget *labelWidget;
  GtkWidget *parent;

  labelWidget = gtk_label_new 
    (AutoGL_INSIDE_GetPropertyLabel (property));
  parent = CurrentPanel;
  gtk_box_pack_start (GTK_BOX(parent), labelWidget, FALSE, FALSE, 0);
  gtk_widget_show (labelWidget);
}



/* Button */

static void ButtonCallback (GtkWidget *widget, gpointer data) 
{
  AutoGL_Property *property = (AutoGL_Property *)data;

  AutoGL_INSIDE_InvokeCallback (property);
}

static void AddButton (AutoGL_Property *property) 
{
  GtkWidget *button;
  GtkWidget *parent;

  button = gtk_button_new_with_label 
    (AutoGL_INSIDE_GetPropertyLabel (property));
  parent = CurrentPanel;
  gtk_box_pack_start (GTK_BOX(parent), button, FALSE, FALSE, 0);
  gtk_signal_connect 
    (GTK_OBJECT(button), "clicked", 
     GTK_SIGNAL_FUNC(ButtonCallback), 
     (gpointer)property);
  gtk_widget_show (button);
}



/* Toggle */

static void CheckButtonCallback (GtkWidget *widget, gpointer data) 
{
  int *flag = (int*)data;

  if (GTK_TOGGLE_BUTTON(widget)->active) {
    *flag = 1;
  } else {
    *flag = 0;
  }
}

static void AddToggle (AutoGL_Property *property) 
{
  GtkWidget *check;
  GtkWidget *parent;

  check = gtk_check_button_new_with_label 
    (AutoGL_INSIDE_GetPropertyLabel (property));
  parent = CurrentPanel;
  gtk_box_pack_start (GTK_BOX(parent), check, FALSE, FALSE, 0);

  if (*property->booleanValue) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check), TRUE);
  } else {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check), FALSE);
  }

  gtk_signal_connect 
    (GTK_OBJECT(check), "toggled", 
     GTK_SIGNAL_FUNC(CheckButtonCallback), 
     (gpointer)property->booleanValue);
  gtk_widget_show (check);
}



/* Field */

static void EntryCallback (GtkWidget *widget, gpointer data) 
{
  AutoGL_Property *property = (AutoGL_Property*)data;
  static char buf[256];

  strcpy (buf, gtk_entry_get_text (GTK_ENTRY(widget)));
  AutoGL_INSIDE_SetPropertyString (property, buf);
}

static void AddField (AutoGL_Property *property) 
{
  static char buf[256];

  GtkWidget *hbox;
  GtkWidget *parent;
  GtkWidget *labelWidget;
  GtkWidget *entry;

  hbox = gtk_hbox_new (FALSE, 0);
  parent = CurrentPanel;
  gtk_box_pack_start (GTK_BOX(parent), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  labelWidget = gtk_label_new 
    (AutoGL_INSIDE_GetPropertyLabel (property));
  gtk_box_pack_start (GTK_BOX(hbox), labelWidget, FALSE, FALSE, 0);
  gtk_widget_show (labelWidget);

  entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX(hbox), entry, FALSE, FALSE, 0);

  AutoGL_INSIDE_GetPropertyString (property, 
				   buf);
  gtk_entry_set_text (GTK_ENTRY(entry), buf);

  gtk_signal_connect 
    (GTK_OBJECT(entry), "changed", 
     GTK_SIGNAL_FUNC(EntryCallback), (gpointer)property);
  gtk_widget_show (entry);
}



/* Choice */

static void RadioCallback (GtkWidget *widget, gpointer data) 
{
  ChoiceData *choice;
  int count;
  GSList *link;
  GtkWidget *radio;

  choice = (ChoiceData*)data;
  count = g_slist_length (choice->slist);
  for (link = choice->slist; link != NULL; link = link->next) {
    count--;
    radio = (GtkWidget*)link->data;
    if (GTK_TOGGLE_BUTTON(radio)->active) {
      *choice->integerValue = count;
      break;
    }
  }
}

static void AddChoice (AutoGL_Property *property) 
{
  GtkWidget *parent;
  int iItem;
  GtkWidget *choiceFrame;
  GtkWidget *choiceVBox;
  ChoiceData *choice;

  assert (NChoices < MAX_CHOICES);
  choice = &Choices[NChoices];
  NChoices++;

  choice->slist = NULL;
  choice->integerValue = property->integerValue;

  choiceFrame = gtk_frame_new 
    (AutoGL_INSIDE_GetPropertyLabel (property));
  parent = CurrentPanel;
  gtk_box_pack_start (GTK_BOX(parent), choiceFrame, FALSE, FALSE, 0);
  gtk_widget_show (choiceFrame);
  
  choiceVBox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(choiceFrame), choiceVBox);
  gtk_widget_show (choiceVBox);

  for (iItem = 0; iItem < property->nItems; iItem++) {
    GtkWidget *radio;

    radio = gtk_radio_button_new_with_label 
      (choice->slist, property->itemLabels[iItem]);
    gtk_box_pack_start (GTK_BOX(choiceVBox), radio, FALSE, FALSE, 0);
    choice->slist = gtk_radio_button_group (GTK_RADIO_BUTTON(radio));

    if (iItem == *choice->integerValue) {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radio), TRUE);
    } else {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radio), FALSE);
    }

    gtk_signal_connect 
      (GTK_OBJECT(radio), "toggled", 
       GTK_SIGNAL_FUNC(RadioCallback), (gpointer)choice);
    gtk_widget_show (radio);
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

static gint RealizeGlAreaCallback
(GtkWidget *widget)
{
  GdkDrawable *drawable = GTK_WIDGET(widget)->window;
  GdkFont* font;

  glXMakeCurrent(AutoGL_XDisplay, 
		 GDK_WINDOW_XWINDOW(drawable), 
		 AutoGL_GLXcontext);

  AutoGL_INSIDE_RealizeOpenGLWindow ();
  
  /* make display list of character font */
  font = gdk_font_load ("a14");
  if (!font) {
    g_print ("Can`t load font a14 \n");
    exit (1);
  }
  
  glXUseXFont (gdk_font_id (font), 
	       0, 128, 
	       AutoGL_INSIDE_GetOpenGLCharBase ());

  gdk_font_unref (font);

  return TRUE;
}

static gint ResizeGlAreaCallback
(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  AutoGL_INSIDE_SetViewRangeDc 
    (widget->allocation.width, widget->allocation.height);

  return TRUE;
}

static gint RedrawGlAreaCallback 
(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  if (event->count > 0) {
    ViewUpdateIsForced = 0;
    return TRUE;
  }

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  if (!AutoGL_INSIDE_IsSynchronized ()) {
    ViewUpdateIsForced = 0;
    return TRUE;
  }
#endif

  if (AutoGL_INSIDE_IsMaster ()
      || ViewUpdateIsForced) {
    AutoGL_INSIDE_StartDrawingOpenGLWindow ();
    AutoGL_INSIDE_InvokeViewRedrawCallback ();
    AutoGL_INSIDE_EndDrawingOpenGLWindow ();

    /* switch front and back buffers and show rendered image */
    {
      GdkDrawable *drawable = GTK_WIDGET(widget)->window;

      glXSwapBuffers (GDK_WINDOW_XDISPLAY(drawable), 
		      GDK_WINDOW_XWINDOW(drawable));
    }
  }

  ViewUpdateIsForced = 0;
  return TRUE;
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

static gint ButtonPressGlAreaCallback 
(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (event->button == 1) {
    int dcX, dcY, dcZ;

    GetDcOfMouse (&dcX, &dcY, &dcZ,
		  (int)event->x, (int)event->y);
    AutoGL_INSIDE_SetPointingDeviceEvent 
      (AUTOGL_EVENT_POINTING_DEVICE_PRESS, 
       dcX, dcY, dcZ);
    AutoGL_INSIDE_InvokeViewEventCallback ();
  }

  return TRUE;
}

static gint MotionNotifyGlAreaCallback 
(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  int x, y;
  GdkModifierType state;

  if (event->is_hint) {
    gdk_window_get_pointer (event->window, 
			    &x, &y, &state);
  } else {
    x = event->x;
    y = event->y;
    state = event->state;
  }

  if (event->state & GDK_BUTTON1_MASK) {
    int dcX, dcY, dcZ;

    GetDcOfMouse (&dcX, &dcY, &dcZ,
		  x, y);
    AutoGL_INSIDE_SetPointingDeviceEvent 
      (AUTOGL_EVENT_POINTING_DEVICE_DRAG, 
       dcX, dcY, dcZ);
    AutoGL_INSIDE_InvokeViewEventCallback ();
  }
  
  return TRUE;
}

static gint ButtonReleaseGlAreaCallback 
(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (event->button == 1) {
    int dcX, dcY, dcZ;

    GetDcOfMouse (&dcX, &dcY, &dcZ,
		  (int)event->x, (int)event->y);
    AutoGL_INSIDE_SetPointingDeviceEvent 
      (AUTOGL_EVENT_POINTING_DEVICE_RELEASE, 
       dcX, dcY, dcZ);
    AutoGL_INSIDE_InvokeViewEventCallback ();
  }

  return TRUE;
}

static gint KeyPressGlAreaCallback 
(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  AutoGL_INSIDE_SetKeyEvent (event->keyval);
  AutoGL_INSIDE_InvokeViewEventCallback ();

  return TRUE;
}

static void PanelCallback 
(GtkWidget *widget, gpointer data)
{
  int panelId = *(int *)(data);
  
  if (PanelIsShown[panelId]) {
    PanelIsShown[panelId] = 0;
    gtk_widget_hide (PanelWindows[panelId]);
  } else {
    PanelIsShown[panelId] = 1;
    gtk_widget_show (PanelWindows[panelId]);
  }
}

gboolean ViewDeleteCallback(GtkWidget *widget, GdkEvent *event, gpointer data){
                                                /* by Asakawa 2011 */ 
  gtk_main_quit();
  return FALSE;  /* close */
}

static void MakeView (void)
{
  int iPanel;
  int widthDc, heightDc;

  ViewWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  if (AutoGL_INSIDE_IsMaster ()) {
    gtk_window_set_title (GTK_WINDOW(ViewWindow), "View");
  } else {
    gtk_window_set_title (GTK_WINDOW(ViewWindow), "View (slave)");
  }


  /* make menu bar */
  
  ViewVBox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(ViewWindow), ViewVBox);
  gtk_widget_show (ViewVBox);
  
  ViewMenuBar = gtk_menu_bar_new ();
  gtk_box_pack_start (GTK_BOX(ViewVBox), ViewMenuBar, 
		      FALSE, TRUE, 0);
  gtk_widget_show (ViewMenuBar);
  
    
    
  /* make panel menu */
    
  ViewPanelMenuItem = gtk_menu_item_new_with_label ("Panel");
  gtk_menu_bar_append (GTK_MENU_BAR(ViewMenuBar), ViewPanelMenuItem);
  gtk_widget_show (ViewPanelMenuItem);
    
  if (AutoGL_INSIDE_IsMaster ()) {

    ViewPanelMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(ViewPanelMenuItem), 
			       ViewPanelMenu);
  
    for (iPanel = 0; iPanel < NPanels; iPanel++) {
      GtkWidget *menuItem;
      
      menuItem = gtk_menu_item_new_with_label (PanelTitles[iPanel]);
      gtk_signal_connect (GTK_OBJECT(menuItem), "activate",
			  GTK_SIGNAL_FUNC(PanelCallback), 
			  (gpointer)&PanelIds[iPanel]);
      gtk_menu_append (GTK_MENU(ViewPanelMenu), menuItem);
      gtk_widget_show (menuItem);
    }
    
  }



  /* make OpenGL drawing area using GTK GL Area widget */

  {
    int attributes[] = {
      4,  /* GDK_GL_RGBA */
      5,  /* GDK_GL_DOUBLEBUFFER */
      12, /* GDK_GL_DEPTH_SIZE */
      1,
      0   /* GDK_GL_NONE */
    };
    
    XVisualInfo *visualInfo;
    GdkVisual *visual;

    AutoGL_XDisplay = GDK_DISPLAY();
    
    visualInfo = glXChooseVisual (AutoGL_XDisplay, 
				  DefaultScreen (AutoGL_XDisplay), 
				  attributes);
    if (visualInfo == NULL) {
      g_print ("glXChooseVisual \n");
      exit (1);
    }
    
    visual = gdkx_visual_get (visualInfo->visualid);
    
    if (visual == NULL) {
      g_print ("gdkx_visual_get \n");
      exit (1);
    }
    
    AutoGL_GLXcontext = glXCreateContext 
      (AutoGL_XDisplay, visualInfo, 0, True);
    
    if (AutoGL_GLXcontext == NULL) {
      g_print ("glXCreateContext \n");
      exit (1);
    }
    
    XFree (visualInfo);

    gtk_widget_push_colormap(gdk_colormap_new (visual, TRUE));
    gtk_widget_push_visual (visual);

    TopGlArea = GTK_WIDGET (gtk_drawing_area_new ());
    
    gtk_widget_set_double_buffered (TopGlArea, FALSE);

    gtk_widget_pop_visual ();
    gtk_widget_pop_colormap ();
  }

  AutoGL_GetViewRangeDc 
    (&widthDc, &heightDc);
  gtk_drawing_area_size (GTK_DRAWING_AREA(TopGlArea), 
			 widthDc, heightDc);

  gtk_widget_set_events (GTK_WIDGET(TopGlArea),
			 GDK_EXPOSURE_MASK |
			 GDK_BUTTON_PRESS_MASK |
			 GDK_POINTER_MOTION_MASK |
			 GDK_POINTER_MOTION_HINT_MASK |
			 GDK_BUTTON_RELEASE_MASK);
  gtk_signal_connect
    (GTK_OBJECT(TopGlArea), "realize",
     GTK_SIGNAL_FUNC(RealizeGlAreaCallback), NULL);
  gtk_signal_connect 
    (GTK_OBJECT(TopGlArea), "configure_event", 
     GTK_SIGNAL_FUNC(ResizeGlAreaCallback), NULL);

  gtk_signal_connect 
    (GTK_OBJECT(TopGlArea), "map_event", 
     GTK_SIGNAL_FUNC(RedrawGlAreaCallback), NULL);
  gtk_signal_connect 
    (GTK_OBJECT(TopGlArea), "expose_event", 
     GTK_SIGNAL_FUNC(RedrawGlAreaCallback), NULL);

  if (AutoGL_INSIDE_IsMaster ()) {
    gtk_signal_connect 
      (GTK_OBJECT(TopGlArea), "button_press_event",
       GTK_SIGNAL_FUNC(ButtonPressGlAreaCallback), NULL);
    gtk_signal_connect 
      (GTK_OBJECT(TopGlArea), "motion_notify_event",
       GTK_SIGNAL_FUNC(MotionNotifyGlAreaCallback), NULL);
    gtk_signal_connect 
      (GTK_OBJECT(TopGlArea), "button_release_event",
       GTK_SIGNAL_FUNC(ButtonReleaseGlAreaCallback), NULL);
    
    gtk_signal_connect 
      (GTK_OBJECT(ViewWindow), "key_press_event",
       GTK_SIGNAL_FUNC(KeyPressGlAreaCallback), NULL);
  }

  gtk_signal_connect
    (GTK_OBJECT(ViewWindow), "delete_event",
     GTK_SIGNAL_FUNC(ViewDeleteCallback), NULL);  /* by Asakawa 2011 */ 

  gtk_box_pack_start (GTK_BOX(ViewVBox), TopGlArea, TRUE, TRUE, 0);
  gtk_widget_show (TopGlArea);

  gtk_widget_show (ViewWindow);
}



void AutoGL_INSIDE_ResizeView (int widthDc, int heightDc)
{
  AutoGL_INSIDE_SetViewRangeDc (widthDc, heightDc);

  /* NEED TO REDEFINE */
  /* how to resize view window ??? */

#if 0
  gtk_drawing_area_size(GTK_DRAWING_AREA(TopGlArea), 
			widthDc, heightDc);
#endif
}

void AutoGL_INSIDE_UpdateView (void)
{
  GdkRectangle updateRectangle;

  ViewUpdateIsForced = 1;

  updateRectangle.x = 0;
  updateRectangle.y = 0;
  updateRectangle.width = TopGlArea->allocation.width;
  updateRectangle.height = TopGlArea->allocation.height;
  gtk_widget_draw (TopGlArea, &updateRectangle);
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

#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)

static gint SynchronizeTimeoutId;

static gint SynchronizeCallback (gpointer data)
{
  gtk_timeout_remove (SynchronizeTimeoutId);
  
  AutoGL_INSIDE_Synchronize ();
  
  return 1;
}

static void SetUpSynchronizationTimer (void)
{
  /* synchronize after waiting 10 seconds */
  SynchronizeTimeoutId 
    = gtk_timeout_add (10000, (GtkFunction)SynchronizeCallback, NULL);
}

#endif

static gint TimeoutCallback (gpointer data)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  if (!AutoGL_INSIDE_IsSynchronized ()) {
    return 1;
  }
#endif

  AutoGL_INSIDE_InvokeIdleEventCallback ();
  return 1;
}

static void SetUpTimer (void)
{
#if defined(AUTOGL_MP) || defined (AUTOGL_MMX)
  SetUpSynchronizationTimer ();
#endif

  if (AutoGL_INSIDE_IsMaster ()) {
    if (AutoGL_INSIDE_IdleEventIsEnabled ()) {
      /* call each 10 msec */
      gtk_timeout_add (10, (GtkFunction)TimeoutCallback, NULL);
    }
  }
}






/* Main Routine */

static void Initialize (int argc, char *argv[])
{
  if (argc == 0) {
    gtk_init (NULL, NULL);
  } else {
    gtk_init (&argc, &argv);
  }

  SetUpTimer ();
  if (AutoGL_INSIDE_IsMaster ()) {
    MakePanels ();
  }
  MakeView ();
}

void AutoGL_MainLoop (void) 
{
  gtk_main ();
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
