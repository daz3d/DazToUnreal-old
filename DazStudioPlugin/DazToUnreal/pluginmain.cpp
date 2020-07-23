// Copyright (C) 2018-2020 David Vodhanel All Rights Reserved.

#include "dzplugin.h"
#include "dzapp.h"

#include "version.h"
#include "daztounrealaction.h"
#include "daztounrealdialog.h"

DZ_PLUGIN_DEFINITION("DazToUnreal");

DZ_PLUGIN_AUTHOR("Code Wizards");

DZ_PLUGIN_VERSION(PLUGIN_MAJOR, PLUGIN_MINOR, PLUGIN_REV, PLUGIN_BUILD);

DZ_PLUGIN_DESCRIPTION(QString(
	"<a href=\"%1/aFirstPlugin/index.htm\">Documentation</a><br><br>"
	"This plugin is a simple example provided with the DAZ Studio "
	"SDK. It provides developers an example of the basics with "
	"regard to project layout, subclassing DzAction, standard "
	"dialog creation and button layout."
).arg(dzApp->getDocumentationPath()));

DZ_PLUGIN_CLASS_GUID(DazToUnrealAction, 569D1102-15DE-44EA-8C8B-A1506297DEC3);
//DZ_PLUGIN_CLASS_GUID(DazToUnrealDialog, 383931F3-377F-4A02-8D4F-CAC810B1DBA1);
