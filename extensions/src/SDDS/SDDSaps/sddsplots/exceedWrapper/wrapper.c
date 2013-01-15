/*
 *
 $Log: not supported by cvs2svn $
 *
 */
#include <X11/Intrinsic.h>
#include <X11/XlibXtra.h>
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>

__declspec(dllexport) void HCLXmInit_BC(void);
void HCLXmInit_BC()
{
  HCLXmInit();
}

extern __declspec(dllexport) Widget XmCreateMainWindow_BC( 
                        Widget parent,
                        char *name,
                        ArgList args,
                        Cardinal argCount);
extern Widget XmCreateMainWindow_BC( 
                        Widget parent,
                        char *name,
                        ArgList args,
                        Cardinal argCount)
{
  return XmCreateMainWindow(parent,name,args,argCount);
}

extern __declspec(dllexport) Widget XmCreateMenuBar_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac);
extern Widget XmCreateMenuBar_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreateMenuBar(p,name,al,ac);
}

extern __declspec(dllexport) Widget XmCreateCascadeButton_BC( 
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac);
extern Widget XmCreateCascadeButton_BC( 
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreateCascadeButton(parent,name,al,ac);
}

extern __declspec(dllexport) Widget XmCreateForm_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreateForm_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount)
{
  return XmCreateForm(parent,name,arglist,argcount);
}

extern __declspec(dllexport) Widget XmCreateDrawingArea_BC( 
                        Widget p,
                        String name,
                        ArgList args,
                        Cardinal n);
extern Widget XmCreateDrawingArea_BC( 
                        Widget p,
                        String name,
                        ArgList args,
                        Cardinal n)
{
  return XmCreateDrawingArea(p,name,args,n);
}

extern __declspec(dllexport) void XmMainWindowSetAreas_BC( 
                        Widget w,
                        Widget menu,
                        Widget command,
                        Widget hscroll,
                        Widget vscroll,
                        Widget wregion);
extern void XmMainWindowSetAreas_BC( 
                        Widget w,
                        Widget menu,
                        Widget command,
                        Widget hscroll,
                        Widget vscroll,
                        Widget wregion)
{
  XmMainWindowSetAreas(w,menu,command,hscroll,vscroll,wregion);
}

extern __declspec(dllexport) Widget XmCreateWarningDialog_BC( 
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac);
extern Widget XmCreateWarningDialog_BC( 
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreateWarningDialog(parent,name,al,ac);
}

extern __declspec(dllexport) void XmUpdateDisplay_BC( 
                        Widget w);
extern void XmUpdateDisplay_BC( 
                        Widget w)
{
  XmUpdateDisplay(w);
}

extern __declspec(dllexport) char * XmTextGetString_BC( 
                        Widget widget);
extern char * XmTextGetString_BC( 
                        Widget widget)
{
  return XmTextGetString(widget);
}

extern __declspec(dllexport) Widget XmCreateScrolledText_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreateScrolledText_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount)
{
  return XmCreateScrolledText(parent,name,arglist,argcount);
}

extern __declspec(dllexport) void XmTextSetString_BC( 
                        Widget widget,
                        char *value);
extern void XmTextSetString_BC( 
                        Widget widget,
                        char *value)
{
  XmTextSetString(widget,value);
}

extern __declspec(dllexport) Widget XmCreatePulldownMenu_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreatePulldownMenu_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreatePulldownMenu(p,name,al,ac);
}

extern __declspec(dllexport) Widget XmCreateOptionMenu_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac);
extern Widget XmCreateOptionMenu_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreateOptionMenu(p,name,al,ac);
}

extern __declspec(dllexport) XmString XmStringCreateSimple_BC( 
                        char *text);
extern XmString XmStringCreateSimple_BC( 
                        char *text)
{
  return XmStringCreateSimple(text);
}

extern __declspec(dllexport) Widget XmCreatePromptDialog_BC( 
                        Widget ds_p,
                        String name,
                        ArgList sb_args,
                        Cardinal sb_n);
extern Widget XmCreatePromptDialog_BC( 
                        Widget ds_p,
                        String name,
                        ArgList sb_args,
                        Cardinal sb_n)
{
  return XmCreatePromptDialog(ds_p,name,sb_args,sb_n);
}

extern __declspec(dllexport) Widget XmSelectionBoxGetChild_BC( 
			Widget sb,
#if NeedWidePrototypes
                        unsigned int which
#else
                        unsigned char which
#endif /* NeedWidePrototypes */
			);
extern Widget XmSelectionBoxGetChild_BC( 
                        Widget sb,
#if NeedWidePrototypes
                        unsigned int which
#else
                        unsigned char which
#endif /* NeedWidePrototypes */
			)
{
  return XmSelectionBoxGetChild(sb,which);
}

extern __declspec(dllexport) Boolean XmStringGetLtoR_BC( 
                        XmString string,
                        XmStringCharSet charset,
                        char **text);
extern Boolean XmStringGetLtoR_BC( 
                        XmString string,
                        XmStringCharSet charset,
                        char **text)
{
  return XmStringGetLtoR(string,charset,text);
}

extern __declspec(dllexport) Boolean XmToggleButtonGetState_BC( 
			Widget w);
extern Boolean XmToggleButtonGetState_BC( 
		        Widget w)
{
  return XmToggleButtonGetState(w);
}

extern __declspec(dllexport) Widget XmCreateFormDialog_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreateFormDialog_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount)
{
  return XmCreateFormDialog(parent,name,arglist,argcount);
}

extern __declspec(dllexport) Widget XmCreateSeparator_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreateSeparator_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount)
{
  return XmCreateSeparator(parent,name,arglist,argcount);
}

extern __declspec(dllexport) Widget XmCreateTextField_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreateTextField_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount)
{
  return XmCreateTextField(parent,name,arglist,argcount);
}

extern __declspec(dllexport) XmString XmStringCreate_BC( 
                        char *text,
                        XmStringCharSet charset);
extern XmString XmStringCreate_BC( 
                        char *text,
                        XmStringCharSet charset)
{
  return XmStringCreate(text,charset);
}

extern __declspec(dllexport) Widget XmCreateLabel_BC( 
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount);
extern Widget XmCreateLabel_BC( 
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount)
{
  return XmCreateLabel(parent,name,arglist,argCount);
}

extern __declspec(dllexport) Widget XmCreateSimpleRadioBox_BC( 
                        Widget parent,
                        String name,
                        ArgList args,
                        Cardinal arg_count);
extern Widget XmCreateSimpleRadioBox_BC( 
                        Widget parent,
                        String name,
                        ArgList args,
                        Cardinal arg_count)
{
  return XmCreateSimpleRadioBox(parent,name,args,arg_count);
}

extern __declspec(dllexport) Widget XmCreateToggleButton_BC( 
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount);
extern Widget XmCreateToggleButton_BC( 
                        Widget parent,
                        char *name,
                        Arg *arglist,
                        Cardinal argCount)
{
  return XmCreateToggleButton(parent,name,arglist,argCount);
}

extern __declspec(dllexport) void XmToggleButtonSetState_BC( 
                        Widget w,
#if NeedWidePrototypes
                        int newstate,
                        int notify
#else
                        Boolean newstate,
                        Boolean notify
#endif /* NeedWidePrototypes */
			);
extern void XmToggleButtonSetState_BC( 
                        Widget w,
#if NeedWidePrototypes
                        int newstate,
                        int notify
#else
                        Boolean newstate,
                        Boolean notify
#endif /* NeedWidePrototypes */
			)
{
  XmToggleButtonSetState(w,newstate,notify);
}

extern __declspec(dllexport) Widget XmCreateRowColumn_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac);
extern Widget XmCreateRowColumn_BC( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreateRowColumn(p,name,al,ac);
}

extern __declspec(dllexport) Widget XmCreatePushButton_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount);
extern Widget XmCreatePushButton_BC( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount)
{
  return XmCreatePushButton(parent,name,arglist,argcount);
}

extern __declspec(dllexport) char * XmTextFieldGetString_BC( 
                        Widget w);
extern char * XmTextFieldGetString_BC( 
                        Widget w)
{
  return XmTextFieldGetString(w);
}

extern __declspec(dllexport) XmString XmStringCreateLtoR_BC( 
                        char *text,
                        XmStringCharSet charset);
extern XmString XmStringCreateLtoR_BC( 
                        char *text,
                        XmStringCharSet charset)
{
  return XmStringCreateLtoR(text,charset);
}

extern __declspec(dllexport) Widget XmCreateInformationDialog_BC( 
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac);
extern Widget XmCreateInformationDialog_BC( 
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac)
{
  return XmCreateInformationDialog(parent,name,al,ac);
}

extern __declspec(dllexport) void XmStringFree_BC( 
                        XmString string);
extern void XmStringFree_BC( 
                        XmString string)
{
  XmStringFree(string);
}

extern __declspec(dllexport) Widget XmMessageBoxGetChild_BC( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int child
#else
                        unsigned char child
#endif /* NeedWidePrototypes */
			);
extern Widget XmMessageBoxGetChild_BC( 
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int child
#else
                        unsigned char child
#endif /* NeedWidePrototypes */
			)
{
  return XmMessageBoxGetChild(widget,child);
}
