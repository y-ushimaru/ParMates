/********************************************************************
 Copyright (C) 2002-2014 Shinobu Yoshimura, University of Tokyo,
 the Japan Society for the Promotion of Science (JSPS)
 All Rights Reserved
 *********************************************************************/
/* AutoGL */

#ifndef __AUTOGL_UIMS_INSIDE__
#define __AUTOGL_UIMS_INSIDE__



#define AUTOGL_MAX_PROPERTY_ITEMS 10
#define AUTOGL_PROPERTY_NAME_LENGTH 80
#define AUTOGL_PROPERTY_LABEL_LENGTH 80

typedef enum {
  AUTOGL_PROPERTY_GROUP, 
  AUTOGL_PROPERTY_COMMENT, 
  AUTOGL_PROPERTY_CALLBACK, 
  AUTOGL_PROPERTY_BOOLEAN, 
  AUTOGL_PROPERTY_STRING, 
  AUTOGL_PROPERTY_INTEGER,
  AUTOGL_PROPERTY_REAL
} AutoGL_PropertyType;

typedef struct {
  AutoGL_PropertyType type;
  char name[AUTOGL_PROPERTY_NAME_LENGTH];
  char label[AUTOGL_PROPERTY_LABEL_LENGTH];
  AutoGL_CallbackType callback;
  int *booleanValue;
  char *stringValue;
  int stringLength;
  int *integerValue;
  int minIntegerValue;
  int maxIntegerValue;
  int nItems;
  char itemLabels[AUTOGL_MAX_PROPERTY_ITEMS][AUTOGL_PROPERTY_LABEL_LENGTH];
  double *realValue;
  double minRealValue;
  double maxRealValue;
} AutoGL_Property;

int AutoGL_INSIDE_GetNPropertys (void);

AutoGL_Property *AutoGL_INSIDE_GetProperty (int propertyId);

char *AutoGL_INSIDE_GetPropertyLabel 
(AutoGL_Property *property);

void AutoGL_INSIDE_SetPropertyString 
(AutoGL_Property *property, 
 const char buf[]);

void AutoGL_INSIDE_GetPropertyString 
(AutoGL_Property *property, 
 char buf_OUT[]);

void AutoGL_INSIDE_CallCallback 
(AutoGL_Property *property);

AutoGL_CallbackType AutoGL_INSIDE_GetViewRedrawCallback (void);

AutoGL_CallbackType AutoGL_INSIDE_GetViewEventCallback (void);

int AutoGL_INSIDE_IdleEventIsEnabled (void);

AutoGL_CallbackType AutoGL_INSIDE_GetIdleEventCallback (void);

AutoGL_CallbackType AutoGL_INSIDE_GetBatchProcessCallback (void);

void AutoGL_INSIDE_CallViewRedrawCallback (void);

void AutoGL_INSIDE_CallViewEventCallback (void);

void AutoGL_INSIDE_CallIdleEventCallback (void);

void AutoGL_INSIDE_CallBatchProcessCallback (void);


#endif  /* __AUTOGL_UIMS_INSIDE__ */







