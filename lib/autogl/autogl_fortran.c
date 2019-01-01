/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* autogl_fortran.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "autogl.h"



#ifdef AUTOGL_FORTRAN_BINDING_DOUBLE_UNDERSCORE
#define BIND(name) name##__
#endif

#ifdef AUTOGL_FORTRAN_BINDING_UNDERSCORE
#define BIND(name) name##_
#endif



extern int main (int argc, char *argv[]);

extern void BIND(autogl_setup) (void);
extern void BIND(autogl_redraw) (void);
extern void BIND(autogl_idle) (void);
extern void BIND(autogl_handle_event) (void);
extern void BIND(autogl_callback) (int*);

static char String[256];

static void SetString (const char *str, int length)
{
  int i;
  
  assert(length < 256 - 1);
  for (i = 0; i < length; i++) {
    String[i] = str[i];
  } 
  String[i] = 0;
}



/* Graphics */

void BIND(autogl_set_background_color)
(double *red, double *green, double *blue)
{
  AutoGL_SetBackgroundColor (*red, *green, *blue);
}

void BIND(autogl_turn_on_specular) (void)
{
  AutoGL_TurnOnSpecular ();
}

void BIND(autogl_turn_on_transparency) (void)
{
  AutoGL_TurnOnTransparency ();
}

void BIND(autogl_set_color)
(double *red, double *green, double *blue)
{
  AutoGL_SetColor (*red, *green, *blue);
}

void BIND(autogl_draw_line)
(double *x0, double *y0, double *z0,    
 double *x1, double *y1, double *z1)
{
  AutoGL_DrawLine 
    (*x0, *y0, *z0,    
     *x1, *y1, *z1); 
}

void BIND(autogl_draw_triangle)
(double *x0, double *y0, double *z0,  
 double *x1, double *y1, double *z1,  
 double *x2, double *y2, double *z2)
{
  AutoGL_DrawTriangle 
    (*x0, *y0, *z0,  
     *x1, *y1, *z1,  
     *x2, *y2, *z2);
}
  
void BIND(autogl_draw_color_interpolated_triangle)
(double *x0, double *y0, double *z0, double *red0, double *green0, double *blue0,  
 double *x1, double *y1, double *z1, double *red1, double *green1, double *blue1,  
 double *x2, double *y2, double *z2, double *red2, double *green2, double *blue2)
{
  AutoGL_DrawColorInterpolatedTriangle 
    (*x0, *y0, *z0, *red0, *green0, *blue0, 
     *x1, *y1, *z1, *red1, *green1, *blue1,  
     *x2, *y2, *z2, *red2, *green2, *blue2); 
}

void BIND(autogl_draw_normal_interpolated_triangle)
(double *x0, double *y0, double *z0, double *nx0, double *ny0, double *nz0,  
 double *x1, double *y1, double *z1, double *nx1, double *ny1, double *nz1,  
 double *x2, double *y2, double *z2, double *nx2, double *ny2, double *nz2)
{
  AutoGL_DrawColorInterpolatedTriangle 
    (*x0, *y0, *z0, *nx0, *ny0, *nz0, 
     *x1, *y1, *z1, *nx1, *ny1, *nz1,  
     *x2, *y2, *z2, *nx2, *ny2, *nz2); 
}

void BIND(autogl_draw_quadrangle)
(double *x0, double *y0, double *z0,
 double *x1, double *y1, double *z1,
 double *x2, double *y2, double *z2,
 double *x3, double *y3, double *z3)
{
  AutoGL_DrawQuadrangle 
    (*x0, *y0, *z0,
     *x1, *y1, *z1,
     *x2, *y2, *z2,
     *x3, *y3, *z3);
}

void BIND(autogl_draw_string)
(double *x, double *y, double *z,
 const char *str, int length)
{
  SetString (str, length);
  AutoGL_DrawString 
    (*x, *y, *z,
     String);
}

void BIND(autogl_open_display_list) (void)
{
  AutoGL_OpenDisplayList ();
}

void BIND(autogl_close_display_list) (void)
{
  AutoGL_CloseDisplayList ();
}

void BIND(autogl_draw_display_list) (void)
{
  AutoGL_DrawDisplayList ();
}

void BIND(autogl_set_image_file_name)
(const char *fileName, int length)
{
  SetString (fileName, length);
  AutoGL_SetImageFileName (String);
}

void BIND(autogl_set_view_size) (double *size)
{
  AutoGL_SetViewSize (*size);
}

void BIND(autogl_set_view_center)
(double *x, double *y, double *z)
{
  AutoGL_SetViewCenter (*x, *y, *z);
}

void BIND(autogl_set_view_direction) 
(double *x, double *y, double *z)
{
  AutoGL_SetViewDirection (*x, *y, *z);
}

void BIND(autogl_set_view_up_vector)
(double *x, double *y, double *z)
{
  AutoGL_SetViewUpVector (*x, *y, *z);
}

void BIND(autogl_set_perspective_view_flag)
(int *flag)
{
  AutoGL_SetPerspectiveViewFlag (*flag);
}

void BIND(autogl_get_view_size) (double *size)
{
  *size = AutoGL_GetViewSize ();
}

void BIND(autogl_get_view_center)
(double *x_OUT, double *y_OUT, double *z_OUT)
{
  AutoGL_GetViewCenter 
    (x_OUT, y_OUT, z_OUT);
}

void BIND(autogl_get_view_direction)
(double *x_OUT, double *y_OUT, double *z_OUT)
{
  AutoGL_GetViewDirection 
    (x_OUT, y_OUT, z_OUT);
}

void BIND(autogl_get_view_up_vector)
(double *x_OUT, double *y_OUT, double *z_OUT)
{
  AutoGL_GetViewUpVector 
    (x_OUT, y_OUT, z_OUT);
}

void BIND(autogl_get_view_range_dc)
(int *widthDc_OUT, int *heightDc_OUT)
{
  AutoGL_GetViewRangeDc 
    (widthDc_OUT, heightDc_OUT);
}

void BIND(autogl_get_vc_of_dc)
(double *vcX_OUT, double *vcY_OUT, double *vcZ_OUT,
 int *dcX, int *dcY, int *dcZ)
{
  AutoGL_GetVcOfDc 
    (vcX_OUT, vcY_OUT, vcZ_OUT,
     *dcX, *dcY, *dcZ);
}

void BIND(autogl_get_position_of_vc)
(double *x_OUT, double *y_OUT, double *z_OUT,
 double *vcX, double *vcY, double *vcZ)
{
  AutoGL_GetPositionOfVc 
    (x_OUT, y_OUT, z_OUT,
     *vcX, *vcY, *vcZ);
}
  
void BIND(autogl_get_position_of_dc) 
(double *x_OUT, double *y_OUT, double *z_OUT,
 int *dcX, int *dcY, int *dcZ)
{
  AutoGL_GetPositionOfDc 
    (x_OUT, y_OUT, z_OUT,
     *dcX, *dcY, *dcZ);
}

void BIND(autogl_get_perspective_view_flag)
(int *flag)
{
  *flag = AutoGL_GetPerspectiveViewFlag ();
}

void BIND(autogl_start_selection)
(int *x, int *y, int *range)
{
  AutoGL_StartSelection (*x, *y, *range);
}

void BIND(autogl_set_selection_id)
(int *id)
{
  AutoGL_SetSelectionId (*id);
}

void BIND(autogl_end_selection) (void)
{
  AutoGL_EndSelection ();
}

void BIND(autogl_get_selected_id)
(int *id)
{
  *id = AutoGL_GetSelectedId ();
}

void BIND(autogl_get_view_event)
(int *type)
{
  *type = AutoGL_GetViewEvent ();
}

void BIND(autogl_get_pointing_device_position_dc)
(int *dcX, int *dcY, int *dcZ)
{
  AutoGL_GetPointingDevicePositionDc 
    (dcX, dcY, dcZ);
}
void BIND(autogl_get_key_char)
(int *ch)
{
  *ch = AutoGL_GetKeyChar ();
}



/* UIMS */

void BIND(autogl_add_group)
(const char *name, int length)
{
  SetString (name, length);
  AutoGL_AddGroup (String);
}

void BIND(autogl_add_comment) (void)
{
  AutoGL_AddComment ();
}

void BIND(autogl_add_boolean) 
(int *value_IO, 
 const char *name, int length)
{
  SetString (name, length);
  AutoGL_AddBoolean 
    (value_IO, 
     String);
}

void BIND(autogl_add_string) 
(char *value_IO, 
 const char *name, 
 int *maxLength, 
 int valueLength, int nameLength)
{
  /* NEED TO REDEFINE */
  /* FORTRAN string is not null-terminated string */
  /* it is filled with space key */

  SetString (name, nameLength);
  AutoGL_AddString 
    (value_IO, 
     String, *maxLength);
}

void BIND(autogl_add_integer)
(int *value_IO, 
 const char *name, int length)
{
  SetString (name, length);
  AutoGL_AddInteger 
    (value_IO, 
     String);
}

void BIND(autogl_set_integer_range)
(int *minValue, int *maxValue)
{
  AutoGL_SetIntegerRange 
    (*minValue, *maxValue);
}

void BIND(autogl_add_integer_item)
(const char *label, int length)
{
  SetString (label, length);
  AutoGL_AddIntegerItem 
    (String);
}

void BIND(autogl_add_real)
(double *value_IO, 
 const char *name, int length)
{
  SetString (name, length);
  AutoGL_AddReal 
    (value_IO, 
     String);
}

void BIND(autogl_set_real_range)
(double *minValue, double *maxValue)
{
  AutoGL_SetRealRange 
    (*minValue, *maxValue);
}

void BIND(autogl_set_label)
(const char *label, int length)
{
  SetString (label, length);
  AutoGL_SetLabel 
    (String);
}

#define MAX_BUTTONS 30
static void Callback0 (void)  {  int id = 0;  BIND(autogl_callback) (&id);  }
static void Callback1 (void)  {  int id = 1;  BIND(autogl_callback) (&id);  }
static void Callback2 (void)  {  int id = 2;  BIND(autogl_callback) (&id);  }
static void Callback3 (void)  {  int id = 3;  BIND(autogl_callback) (&id);  }
static void Callback4 (void)  {  int id = 4;  BIND(autogl_callback) (&id);  }
static void Callback5 (void)  {  int id = 5;  BIND(autogl_callback) (&id);  }
static void Callback6 (void)  {  int id = 6;  BIND(autogl_callback) (&id);  }
static void Callback7 (void)  {  int id = 7;  BIND(autogl_callback) (&id);  }
static void Callback8 (void)  {  int id = 8;  BIND(autogl_callback) (&id);  }
static void Callback9 (void)  {  int id = 9;  BIND(autogl_callback) (&id);  }
static void Callback10 (void)  {  int id = 10;  BIND(autogl_callback) (&id);  }
static void Callback11 (void)  {  int id = 11;  BIND(autogl_callback) (&id);  }
static void Callback12 (void)  {  int id = 12;  BIND(autogl_callback) (&id);  }
static void Callback13 (void)  {  int id = 13;  BIND(autogl_callback) (&id);  }
static void Callback14 (void)  {  int id = 14;  BIND(autogl_callback) (&id);  }
static void Callback15 (void)  {  int id = 15;  BIND(autogl_callback) (&id);  }
static void Callback16 (void)  {  int id = 16;  BIND(autogl_callback) (&id);  }
static void Callback17 (void)  {  int id = 17;  BIND(autogl_callback) (&id);  }
static void Callback18 (void)  {  int id = 18;  BIND(autogl_callback) (&id);  }
static void Callback19 (void)  {  int id = 19;  BIND(autogl_callback) (&id);  }
static void Callback20 (void)  {  int id = 20;  BIND(autogl_callback) (&id);  }
static void Callback21 (void)  {  int id = 21;  BIND(autogl_callback) (&id);  }
static void Callback22 (void)  {  int id = 22;  BIND(autogl_callback) (&id);  }
static void Callback23 (void)  {  int id = 23;  BIND(autogl_callback) (&id);  }
static void Callback24 (void)  {  int id = 24;  BIND(autogl_callback) (&id);  }
static void Callback25 (void)  {  int id = 25;  BIND(autogl_callback) (&id);  }
static void Callback26 (void)  {  int id = 26;  BIND(autogl_callback) (&id);  }
static void Callback27 (void)  {  int id = 27;  BIND(autogl_callback) (&id);  }
static void Callback28 (void)  {  int id = 28;  BIND(autogl_callback) (&id);  }
static void Callback29 (void)  {  int id = 29;  BIND(autogl_callback) (&id);  }

void BIND(autogl_set_button)
(int *id, const char *label, int length)
{
  assert(0 <= *id);
  assert(*id < MAX_BUTTONS);
  SetString (label, length);

  assert(MAX_BUTTONS == 30);
  switch (*id) {
  case 0:  AutoGL_AddCallback (Callback0, String);  break;
  case 1:  AutoGL_AddCallback (Callback1, String);  break;
  case 2:  AutoGL_AddCallback (Callback2, String);  break;
  case 3:  AutoGL_AddCallback (Callback3, String);  break;
  case 4:  AutoGL_AddCallback (Callback4, String);  break;
  case 5:  AutoGL_AddCallback (Callback5, String);  break;
  case 6:  AutoGL_AddCallback (Callback6, String);  break;
  case 7:  AutoGL_AddCallback (Callback7, String);  break;
  case 8:  AutoGL_AddCallback (Callback8, String);  break;
  case 9:  AutoGL_AddCallback (Callback9, String);  break;
  case 10:  AutoGL_AddCallback (Callback10, String);  break;
  case 11:  AutoGL_AddCallback (Callback11, String);  break;
  case 12:  AutoGL_AddCallback (Callback12, String);  break;
  case 13:  AutoGL_AddCallback (Callback13, String);  break;
  case 14:  AutoGL_AddCallback (Callback14, String);  break;
  case 15:  AutoGL_AddCallback (Callback15, String);  break;
  case 16:  AutoGL_AddCallback (Callback16, String);  break;
  case 17:  AutoGL_AddCallback (Callback17, String);  break;
  case 18:  AutoGL_AddCallback (Callback18, String);  break;
  case 19:  AutoGL_AddCallback (Callback19, String);  break;
  case 20:  AutoGL_AddCallback (Callback20, String);  break;
  case 21:  AutoGL_AddCallback (Callback21, String);  break;
  case 22:  AutoGL_AddCallback (Callback22, String);  break;
  case 23:  AutoGL_AddCallback (Callback23, String);  break;
  case 24:  AutoGL_AddCallback (Callback24, String);  break;
  case 25:  AutoGL_AddCallback (Callback25, String);  break;
  case 26:  AutoGL_AddCallback (Callback26, String);  break;
  case 27:  AutoGL_AddCallback (Callback27, String);  break;
  case 28:  AutoGL_AddCallback (Callback28, String);  break;
  case 29:  AutoGL_AddCallback (Callback29, String);  break;
  default:
    assert(0);
    break;
  }
}

void BIND(autogl_enable_idle_event) (void)
{
  AutoGL_EnableIdleEvent ();
}

void BIND(autogl_set_idle_event_callback) 
(int *flag)
{
  if (*flag) {
    AutoGL_SetIdleEventCallback (BIND(autogl_idle));
  } else {
    AutoGL_SetIdleEventCallback (NULL);
  }
}



/* GUI */

void BIND(autogl_resize_view) 
(int *widthDc, int *heightDc)
{
  AutoGL_ResizeView (*widthDc, *heightDc);
}

void BIND(autogl_draw_view) (void)
{
  AutoGL_DrawView ();
}

void BIND(autogl_terminate) (void)
{
  AutoGL_Terminate ();
}



/* Utility */

void BIND(autogl_set_panel_for_interpreter) (void)
{
  AutoGL_SetPanelForInterpreter ();
}

void BIND(autogl_save_view_image_to_ppm_file)
(const char *fileName, int length)
{
  SetString (fileName, length);
  AutoGL_SaveViewImageToPPMFile (String);
}

void BIND(autogl_set_panel_for_save) (void)
{
  AutoGL_SetPanelForSave ();
}

void BIND(autogl_draw_string_at_scaled_position)
(double *rx, double *ry,
 const char *str, int length)
{
  SetString (str, length);
  AutoGL_DrawStringAtScaledPosition (*rx, *ry, String);
}

void BIND(autogl_draw_time_dhms)
(double *time)
{
  AutoGL_DrawTimeDHMS (*time);
}



/* Visualization */

void BIND(autogl_clear_contour_color) (void)
{
  AutoGL_ClearContourColor ();
}

void BIND(autogl_add_contour_color_of_grade)
(double *grade,
 double *red, double *green, double *blue)
{
  AutoGL_AddContourColorOfGrade (*grade, 
				 *red, *green, *blue);
}

void BIND(autogl_set_gray_scale_contour_map) (void)
{
  AutoGL_SetGrayScaleContourMap ();
}

void BIND(autogl_set_contour_map_bcgyrm) (void)
{
  AutoGL_SetContourMap_BCGYRM ();
}

void BIND(autogl_set_contour_map_mbcgyr) (void)
{
  AutoGL_SetContourMap_MBCGYR ();
}

void BIND(autogl_set_contour_map_bcgyr) (void)
{
  AutoGL_SetContourMap_BCGYR ();
}

void BIND(autogl_get_contour_color)
(double *red_OUT, double *green_OUT, double *blue_OUT,
 double *grade)
{
  AutoGL_GetContourColor 
    (red_OUT, green_OUT, blue_OUT,
     *grade);
}

void BIND(autogl_draw_contour_triangle)
(double *x0, double *y0, double *z0, double *grade0,  
 double *x1, double *y1, double *z1, double *grade1,  
 double *x2, double *y2, double *z2, double *grade2)
{
  AutoGL_DrawContourTriangle 
    (*x0, *y0, *z0, *grade0,  
     *x1, *y1, *z1, *grade1,  
     *x2, *y2, *z2, *grade2); 
}

void BIND(autogl_draw_contour_map)
(int *sizeDc,
 double *minRange, double *maxRange)
{
  AutoGL_DrawContourMap 
    (*sizeDc, *minRange, *maxRange); 
}

void BIND(autogl_draw_line_contour_map)
(int *sizeDc,
 double *minRange, double *maxRange,
 int *nGrades)
{
  AutoGL_DrawLineContourMap 
    (*sizeDc, *minRange, *maxRange, *nGrades); 
}

void BIND(autogl_draw_band_contour_map)
(int *sizeDc,
 double *minRange, double *maxRange,
 int *nGrades)
{
  AutoGL_DrawBandContourMap 
    (*sizeDc, *minRange, *maxRange, *nGrades); 
}

void BIND(autogl_draw_smooth_contour_triangle)
(double *minrange, double *maxrange,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2)
{
  AutoGL_DrawSmoothContourTriangle 
    (*minrange, *maxrange,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2);
}

void BIND(autogl_draw_line_contour_triangle)
(double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2)
{
  AutoGL_DrawLineContourTriangle 
    (*minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2);
}

void BIND(autogl_draw_band_contour_triangle)
(double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2)
{
  AutoGL_DrawBandContourTriangle 
    (*minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2);
}

void BIND(autogl_draw_smooth_contour_quadrangle)
(double *minrange, double *maxrange,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawSmoothContourQuadrangle 
    (*minrange, *maxrange,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_line_contour_quadrangle)
(double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawLineContourQuadrangle 
    (*minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_band_contour_quadrangle)
(double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawBandContourQuadrangle 
    (*minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_isosurface_tetrahedron)
(double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawIsosurfaceTetrahedron 
    (*minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_section_smooth_contour_tetrahedron)
(double *a, double *b, double *c, double *d,
 double *minrange, double *maxrange,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawSectionSmoothContourTetrahedron 
    (*a, *b, *c, *d, *minrange, *maxrange,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_section_line_contour_tetrahedron)
(double *a, double *b, double *c, double *d,
 double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawSectionLineContourTetrahedron 
    (*a, *b, *c, *d, *minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_section_band_contour_tetrahedron)
(double *a, double *b, double *c, double *d,
 double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3)
{
  AutoGL_DrawSectionBandContourTetrahedron 
    (*a, *b, *c, *d, *minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3);
}

void BIND(autogl_draw_isosurface_hexahedron)
(double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3,
 double *x4, double *y4, double *z4, double *value4,  
 double *x5, double *y5, double *z5, double *value5,  
 double *x6, double *y6, double *z6, double *value6,  
 double *x7, double *y7, double *z7, double *value7)
{
  AutoGL_DrawIsosurfaceHexahedron 
    (*minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3,
     *x4, *y4, *z4, *value4,
     *x5, *y5, *z5, *value5,
     *x6, *y6, *z6, *value6,
     *x7, *y7, *z7, *value7);
}

void BIND(autogl_draw_section_smooth_contour_hexahedron)
(double *a, double *b, double *c, double *d,
 double *minrange, double *maxrange,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3,
 double *x4, double *y4, double *z4, double *value4,  
 double *x5, double *y5, double *z5, double *value5,  
 double *x6, double *y6, double *z6, double *value6,  
 double *x7, double *y7, double *z7, double *value7)
{
  AutoGL_DrawSectionSmoothContourHexahedron 
    (*a, *b, *c, *d, *minrange, *maxrange,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3,
     *x4, *y4, *z4, *value4,
     *x5, *y5, *z5, *value5,
     *x6, *y6, *z6, *value6,
     *x7, *y7, *z7, *value7);
}

void BIND(autogl_draw_section_line_contour_hexahedron)
(double *a, double *b, double *c, double *d,
 double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3,
 double *x4, double *y4, double *z4, double *value4,  
 double *x5, double *y5, double *z5, double *value5,  
 double *x6, double *y6, double *z6, double *value6,  
 double *x7, double *y7, double *z7, double *value7)
{
  AutoGL_DrawSectionLineContourHexahedron 
    (*a, *b, *c, *d, *minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3,
     *x4, *y4, *z4, *value4,
     *x5, *y5, *z5, *value5,
     *x6, *y6, *z6, *value6,
     *x7, *y7, *z7, *value7);
}

void BIND(autogl_draw_section_band_contour_hexahedron)
(double *a, double *b, double *c, double *d,
 double *minrange, double *maxrange,
 int *ngrades,
 double *x0, double *y0, double *z0, double *value0,  
 double *x1, double *y1, double *z1, double *value1,  
 double *x2, double *y2, double *z2, double *value2,  
 double *x3, double *y3, double *z3, double *value3,
 double *x4, double *y4, double *z4, double *value4,  
 double *x5, double *y5, double *z5, double *value5,  
 double *x6, double *y6, double *z6, double *value6,  
 double *x7, double *y7, double *z7, double *value7)
{
  AutoGL_DrawSectionBandContourHexahedron 
    (*a, *b, *c, *d, *minrange, *maxrange, *ngrades,
     *x0, *y0, *z0, *value0,
     *x1, *y1, *z1, *value1,
     *x2, *y2, *z2, *value2,
     *x3, *y3, *z3, *value3,
     *x4, *y4, *z4, *value4,
     *x5, *y5, *z5, *value5,
     *x6, *y6, *z6, *value6,
     *x7, *y7, *z7, *value7);
}



/* Utility 2D */

void BIND(autogl_set_panel_in_view2d) (void)
{
  AutoGL_SetPanelInView2D ();
}

void BIND(autogl_draw_box2d) 
(double *p0X, double *p0Y,
 double *p1X, double *p1Y, 
 double *z)
{
  AutoGL_DrawBox2D 
    (*p0X, *p0Y, *p1X, *p1Y, *z);
}

void BIND(autogl_draw_cross_mark2d) 
(double *pX, double *pY, double *pZ,
 int *sizeDc)
{
  AutoGL_DrawCrossMark2D 
    (*pX, *pY, *pZ, *sizeDc);
}

void BIND(autogl_draw_diamond_mark2d) 
(double *pX, double *pY, double *pZ,
 int *sizeDc, int *isFilled)
{
  AutoGL_DrawDiamondMark2D 
    (*pX, *pY, *pZ, *sizeDc, *isFilled);
}

void BIND(autogl_draw_square_mark2d) 
(double *pX, double *pY, double *pZ,
 int *sizeDc, int *isFilled)
{
  AutoGL_DrawSquareMark2D 
    (*pX, *pY, *pZ, *sizeDc, *isFilled);
}

void BIND(autogl_draw_arrow2d) 
(double *x0, double *y0, 
 double *x1, double *y1, 
 double *z,
 double *tipRatio)
{
  AutoGL_DrawArrow2D 
    (*x0, *y0, *x1, *y1, *z, *tipRatio);
}

void BIND(autogl_set_panel_in_mode2d) (void)
{
  AutoGL_SetPanelInMode2D ();
}

void BIND(autogl_set_mode2d) 
(int *mode)
{
  AutoGL_SetMode2D 
    (*mode);
}

void BIND(autogl_enable_drag_in_mode2d) (void)
{
  AutoGL_EnableDragInMode2D ();
}

void BIND(autogl_get_mode2d) 
(int *mode)
{
  *mode = AutoGL_GetMode2D ();
}

void BIND(autogl_set_default_callback_in_mode2d) (void)
{
  AutoGL_SetDefaultCallbackInMode2D (BIND(autogl_handle_event));
}

void BIND(autogl_handle_default_key_event_in_mode2d) (void)
{
  AutoGL_HandleDefaultKeyEventInMode2D ();
}

void BIND(autogl_pointing_device_is_hit2d) 
(int *flag,
 double *pX, double *pY,
 int *toleranceDc)
{
  *flag = AutoGL_PointingDeviceIsHit2D 
    (*pX, *pY, *toleranceDc);
}



/* Utility 3D */

void BIND(autogl_set_panel_in_view3d) (void)
{
  AutoGL_SetPanelInView3D ();
}

void BIND(autogl_draw_box3d) 
(double *p0X, double *p0Y, double *p0Z,
 double *p1X, double *p1Y, double *p1Z)
{
  AutoGL_DrawBox3D 
    (*p0X, *p0Y, *p0Z, *p1X, *p1Y, *p1Z);
}

void BIND(autogl_draw_cross_mark3d) 
(double *pX, double *pY, double *pZ,
 int *sizeDc)
{
  AutoGL_DrawCrossMark3D 
    (*pX, *pY, *pZ, *sizeDc);
}

void BIND(autogl_draw_diamond_mark3d) 
(double *pX, double *pY, double *pZ,
 int *sizeDc, int *isFilled)
{
  AutoGL_DrawDiamondMark3D 
    (*pX, *pY, *pZ, *sizeDc, *isFilled);
}

void BIND(autogl_draw_cube_mark3d) 
(double *pX, double *pY, double *pZ,
 int *sizeDc, int *isFilled)
{
  AutoGL_DrawCubeMark3D 
    (*pX, *pY, *pZ, *sizeDc, *isFilled);
}

void BIND(autogl_draw_arrow3d) 
(double *x0, double *y0, double *z0,
 double *x1, double *y1, double *z1,
 double *tipRatio)
{
  AutoGL_DrawArrow3D 
    (*x0, *y0, *z0, *x1, *y1, *z1, *tipRatio);
}

void BIND(autogl_draw_cone_arrow3d) 
(double *x0, double *y0, double *z0,
 double *x1, double *y1, double *z1,
 double *tipRatio)
{
  AutoGL_DrawConeArrow3D 
    (*x0, *y0, *z0, *x1, *y1, *z1, *tipRatio);
}

void BIND(autogl_draw_coordinate_system3d) 
(int *sizeDc)
{
  AutoGL_DrawCoordinateSystem3D 
    (*sizeDc);
}

void BIND(autogl_draw_sphere3d) 
(double *centerX, double *centerY, double *centerZ,
 double *radius, 
 int *nDivisions)
{
  AutoGL_DrawSphere3D
    (*centerX, *centerY, *centerZ, *radius, *nDivisions);
}

void BIND(autogl_draw_cylinder3d) 
(double *bottomX, double *bottomY, double *bottomZ,
 double *topX, double *topY, double *topZ,
 double *radius, 
 int *nDivisions)
{
  AutoGL_DrawCylinder3D
    (*bottomX, *bottomY, *bottomZ,
     *topX, *topY, *topZ,
     *radius, 
     *nDivisions);
}

void BIND(autogl_draw_cone3d) 
(double *bottomX, double *bottomY, double *bottomZ,
 double *topX, double *topY, double *topZ,
 double *radius, 
 int *nDivisions)
{
  AutoGL_DrawCone3D
    (*bottomX, *bottomY, *bottomZ,
     *topX, *topY, *topZ,
     *radius, 
     *nDivisions);
}

void BIND(autogl_draw_circle3d) 
(double *centerX, double *centerY, double *centerZ,
 double *normalX, double *normalY, double *normalZ,
 double *radius, 
 int *nDivisions)
{
  AutoGL_DrawCircle3D
    (*centerX, *centerY, *centerZ, 
     *normalX, *normalY, *normalZ,
     *radius, *nDivisions);
}

void BIND(autogl_fill_box3d) 
(double *x0, double *y0, double *z0,
 double *x1, double *y1, double *z1)
{
  AutoGL_FillBox3D
    (*x0, *y0, *z0, 
     *x1, *y1, *z1);
}

void BIND(autogl_set_panel_in_mode3d) (void)
{
  AutoGL_SetPanelInMode3D ();
}

void BIND(autogl_set_mode3d) 
(int *mode)
{
  AutoGL_SetMode3D 
    (*mode);
}

void BIND(autogl_enable_drag_in_mode3d) (void)
{
  AutoGL_EnableDragInMode3D ();
}

void BIND(autogl_get_mode3d) 
(int *mode)
{
  *mode = AutoGL_GetMode3D ();
}

void BIND(autogl_set_default_callback_in_mode3d) (void)
{
  AutoGL_SetDefaultCallbackInMode3D (BIND(autogl_handle_event));
}

void BIND(autogl_handle_default_key_event_in_mode3d) (void)
{
  AutoGL_HandleDefaultKeyEventInMode3D ();
}

void BIND(autogl_pointing_device_is_hit3d) 
(int *flag, double *parameter,
 double *pX, double *pY, double *pZ,
 int *toleranceDc)
{
  *flag = AutoGL_PointingDeviceIsHit3D 
    (parameter,
     *pX, *pY, *pZ, *toleranceDc);
}



#ifdef AUTOGL_FORTRAN

void BIND(autogl_main) (void)
{
  AutoGL_SetViewRedrawCallback (BIND(autogl_redraw));

  AutoGL_Main ();
}

#else

/* NEED TO REDEFINE */
/* Unix only solution */
/* How can I call WinMain() in case of Cygwin ? */
int BIND(MAIN)()
{
  return main (0, NULL);
}

void AutoGL_SetUp (int argc, char *argv[])
{
  AutoGL_SetViewRedrawCallback (BIND(autogl_redraw));
  AutoGL_SetIdleEventCallback (BIND(autogl_idle));

  BIND(autogl_setup) ();
}

#endif

