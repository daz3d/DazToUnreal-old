// Copyright (C) 2018-2020 David Vodhanel All Rights Reserved.

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QToolTip>
#include <QtGui/QWhatsThis>
#include <QtGui/qlineedit.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qfiledialog.h>
#include <QtCore/qsettings.h>
#include <QtGui/qformlayout.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qdesktopservices.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qlistwidget.h>
#include <QtGui/qgroupbox.h>

#include "dzapp.h"
#include "dzscene.h"
#include "dzstyle.h"
#include "dzmainwindow.h"
#include "dzactionmgr.h"
#include "dzaction.h"
#include "dzskeleton.h"

#include "daztounrealdialog.h"
#include "morphselectiondialog.h"
#include "subdivisiondialog.h"

/*****************************
Local definitions
*****************************/
#define DAZ_TO_UNREAL_PLUGIN_NAME		"DazToUnreal"


DazToUnrealDialog::DazToUnrealDialog(QWidget *parent) :
	DzBasicDialog(parent, DAZ_TO_UNREAL_PLUGIN_NAME)
{
	settings = new QSettings("Code Wizards", "DazToUnreal");

	// Declarations
	int margin = style()->pixelMetric(DZ_PM_GeneralMargin);
	int wgtHeight = style()->pixelMetric(DZ_PM_ButtonHeight);
	int btnMinWidth = style()->pixelMetric(DZ_PM_ButtonMinWidth);

	// Set the dialog title
	setWindowTitle(tr("DazToUnreal"));

	QFormLayout* mainLayout = new QFormLayout(this);
	QFormLayout* advancedLayout = new QFormLayout(this);

	// Asset Name
	assetNameEdit = new QLineEdit(this);
	assetNameEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_]*"), this));

	// Intermediate Folder
	QHBoxLayout* intermediateFolderLayout = new QHBoxLayout(this);
	intermediateFolderEdit = new QLineEdit(this);
	intermediateFolderButton = new QPushButton("...", this);
	connect(intermediateFolderButton, SIGNAL(released()), this, SLOT( HandleSelectIntermediateFolderButton() ));

	intermediateFolderLayout->addWidget(intermediateFolderEdit);
	intermediateFolderLayout->addWidget(intermediateFolderButton);

	// Asset Transfer Type
	assetTypeCombo = new QComboBox(this);
	assetTypeCombo->addItem("Skeletal Mesh");
	assetTypeCombo->addItem("Static Mesh");
	assetTypeCombo->addItem("Animation");
	//assetTypeCombo->addItem("Pose");

	// Morphs
	QHBoxLayout* morphsLayout = new QHBoxLayout(this);
	morphsButton = new QPushButton("Choose Morphs", this);
	connect(morphsButton, SIGNAL(released()), this, SLOT(HandleChooseMorphsButton()));
	morphsEnabledCheckBox = new QCheckBox("", this);
	morphsEnabledCheckBox->setMaximumWidth(25);
	morphsLayout->addWidget(morphsEnabledCheckBox);
	morphsLayout->addWidget(morphsButton);
	connect(morphsEnabledCheckBox, SIGNAL(stateChanged(int)), this, SLOT(HandleMorphsCheckBoxChange(int)));

	// Subdivision
	QHBoxLayout* subdivisionLayout = new QHBoxLayout(this);
	subdivisionButton = new QPushButton("Choose Subdivisions", this);
	connect(subdivisionButton, SIGNAL(released()), this, SLOT(HandleChooseSubdivisionsButton()));
	subdivisionEnabledCheckBox = new QCheckBox("", this);
	subdivisionEnabledCheckBox->setMaximumWidth(25);
	subdivisionLayout->addWidget(subdivisionEnabledCheckBox);
	subdivisionLayout->addWidget(subdivisionButton);
	connect(subdivisionEnabledCheckBox, SIGNAL(stateChanged(int)), this, SLOT(HandleSubdivisionCheckBoxChange(int)));

	// Ports
	portEdit = new QLineEdit("32345");
	connect(portEdit, SIGNAL(textChanged(const QString &)), this, SLOT(HandlePortChanged(const QString &)));

	// FBX Version
	fbxVersionCombo = new QComboBox(this);
	fbxVersionCombo->addItem("FBX 2014 -- Binary");
	fbxVersionCombo->addItem("FBX 2014 -- Ascii");
	fbxVersionCombo->addItem("FBX 2013 -- Binary");
	fbxVersionCombo->addItem("FBX 2013 -- Ascii");
	fbxVersionCombo->addItem("FBX 2012 -- Binary");
	fbxVersionCombo->addItem("FBX 2012 -- Ascii");
	fbxVersionCombo->addItem("FBX 2011 -- Binary");
	fbxVersionCombo->addItem("FBX 2011 -- Ascii");
	fbxVersionCombo->addItem("FBX 2010 -- Binary");
	fbxVersionCombo->addItem("FBX 2010 -- Ascii");
	fbxVersionCombo->addItem("FBX 2009 -- Binary");
	fbxVersionCombo->addItem("FBX 2009 -- Ascii");
	connect(fbxVersionCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(HandleFBXVersionChange(const QString &)));

	// Add the widget to the basic dialog
	mainLayout->addRow("Asset Name", assetNameEdit);
	mainLayout->addRow("Asset Type", assetTypeCombo);
	mainLayout->addRow("Enable Morphs", morphsLayout);
	mainLayout->addRow("Enable Subdivision", subdivisionLayout);
	//mainLayout->addRow("Project", projectLayout);
	advancedLayout->addRow("Intermediate Folder", intermediateFolderLayout);
	advancedLayout->addRow("Port", portEdit);
	advancedLayout->addRow("FBX Version", fbxVersionCombo);
	addLayout(mainLayout);

	// Advanced
	advancedSettingsGroupBox = new QGroupBox("Advanced Settings", this);
	advancedSettingsGroupBox->setLayout(advancedLayout);
	addWidget(advancedSettingsGroupBox);


	// Make the dialog fit its contents, with a minimum width, and lock it down
	resize(QSize(500, 0).expandedTo(minimumSizeHint()));
	setFixedWidth(width());
	setFixedHeight(height());

	// Help
	assetNameEdit->setWhatsThis("This is the name the asset will use in Unreal.");
	assetTypeCombo->setWhatsThis("Skeletal Mesh for something with moving parts, like a character\nStatic Mesh for things like props\nAnimation for a character animation.");
	intermediateFolderEdit->setWhatsThis("DazToUnreal will collect the assets in a subfolder under this folder.  Unreal will import them from here.");
	intermediateFolderButton->setWhatsThis("DazToUnreal will collect the assets in a subfolder under this folder.  Unreal will import them from here.");
	portEdit->setWhatsThis("The UDP port used to talk to the DazToUnreal Unreal plugin.\nThis needs to match the port set in the Project Settings in Unreal.\nDefault is 32345.");
	fbxVersionCombo->setWhatsThis("The version of FBX to use when exporting assets.");

	// Load Settings
	if (!settings->value("IntermediatePath").isNull())
	{
		intermediateFolderEdit->setText(settings->value("IntermediatePath").toString());
	}
	else
	{
		QString DefaultPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + "DazToUnreal";
		intermediateFolderEdit->setText(DefaultPath);
	}
	if (!settings->value("Port").isNull())
	{
		portEdit->setText(settings->value("Port").toString());
	}
	if (!settings->value("MorphsEnabled").isNull())
	{
		morphsEnabledCheckBox->setChecked(settings->value("MorphsEnabled").toBool());
	}
	if (!settings->value("FBXExportVersion").isNull())
	{
		int index = fbxVersionCombo->findText(settings->value("FBXExportVersion").toString());
		if (index != -1)
		{
			fbxVersionCombo->setCurrentIndex(index);
		}
	}

	// Set Defaults
	DzNode* Selection = dzScene->getPrimarySelection();
	if (dzScene->getFilename().length() > 0)
	{
		QFileInfo fileInfo = QFileInfo(dzScene->getFilename());
		assetNameEdit->setText(fileInfo.baseName().remove(QRegExp("[^A-Za-z0-9_]")));
	}
	else if(dzScene->getPrimarySelection())
	{
		assetNameEdit->setText(Selection->getLabel().remove(QRegExp("[^A-Za-z0-9_]")));
	}

	if (qobject_cast<DzSkeleton*>(Selection))
	{
		assetTypeCombo->setCurrentIndex(0);
	}
	else
	{
		assetTypeCombo->setCurrentIndex(1);
	}
}

void DazToUnrealDialog::HandleSelectIntermediateFolderButton()
{
	QString directoryName = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
		"/home",
		QFileDialog::ShowDirsOnly
		|QFileDialog::DontResolveSymlinks);

	if (directoryName != NULL)
	{
		intermediateFolderEdit->setText(directoryName);
		settings->setValue("IntermediatePath", directoryName);
	}
}

void DazToUnrealDialog::HandlePortChanged(const QString& port)
{
	settings->setValue("Port", port);
}

void DazToUnrealDialog::HandleChooseMorphsButton()
{
	MorphSelectionDialog *dlg = MorphSelectionDialog::Get(this);
	dlg->exec();
	morphString = dlg->GetMorphString();
	morphMapping = dlg->GetMorphRenaming();
}

void DazToUnrealDialog::HandleChooseSubdivisionsButton()
{
	SubdivisionDialog *dlg = SubdivisionDialog::Get(this);
	dlg->exec();
}

QString DazToUnrealDialog::GetMorphString()
{
	morphMapping = MorphSelectionDialog::Get(this)->GetMorphRenaming();
	return MorphSelectionDialog::Get(this)->GetMorphString();
}

void DazToUnrealDialog::HandleMorphsCheckBoxChange(int state)
{
	settings->setValue("MorphsEnabled", state == Qt::Checked);
}

void DazToUnrealDialog::HandleSubdivisionCheckBoxChange(int state)
{
	settings->setValue("SubdivisionEnabled", state == Qt::Checked);
}

void DazToUnrealDialog::HandleFBXVersionChange(const QString& fbxVersion)
{
	settings->setValue("FBXExportVersion", fbxVersion);
}