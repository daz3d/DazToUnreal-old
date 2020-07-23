// Copyright (C) 2018-2020 David Vodhanel All Rights Reserved.

#include <QtGui/QMessageBox>

#include "dzapp.h"
#include "dzscene.h"
#include "dzexportmgr.h"
#include "DzFileIOSettings.h"
#include "dzexporter.h"
#include "dzmainwindow.h"
#include "dzmaterial.h"
#include "dzproperty.h"
#include "QtCore/qfile.h"
#include "QtCore/qtextstream.h"
#include "dzimageproperty.h"
#include "dzstringproperty.h"
#include "dznumericproperty.h"
#include "dzcolorproperty.h"
#include "dzstringproperty.h"
#include "dzenumproperty.h"
#include "dzboolproperty.h"
#include "dzobject.h"
#include "dzskeleton.h"
#include "dzfigure.h"
#include "dzshape.h"
#include "dzassetmgr.h"
#include "dzuri.h"
#include "dzcontentmgr.h"
#include "dzassetmetadata.h"
#include "dzbone.h"
#include "dzskeleton.h"
#include "dzpresentation.h"

#include "QtCore/qdir.h"
#include <QtGui/qlineedit.h>
#include <QtNetwork/qudpsocket.h>
#include <QtNetwork/qabstractsocket.h>
#include <QtGui/qcheckbox.h>

#include "daztounrealdialog.h"
#include "subdivisiondialog.h"

#include "daztounrealaction.h"

DazToUnrealAction::DazToUnrealAction() :
	DzEditAction("Send To Unreal", "Send the selected character to Unreal.")
{
}


void DazToUnrealAction::executeAction()
{
	// Check if the main window has been created yet.
	// If it hasn't, alert the user and exit early.

	DzMainWindow *mw = dzApp->getInterface();
	if (!mw)
	{
		QMessageBox::warning(0, tr("Error"),
			tr("The main window has not been created yet."), QMessageBox::Ok);

		return;
	}

	// Create and show the dialog. If the user cancels, exit early,
	// otherwise continue on and do the thing that required modal
	// input from the user.

	if (dzScene->getNumSelectedNodes() != 1)
	{
		QMessageBox::warning(0, tr("Error"),
			tr("Please select one Character or Prop to send."), QMessageBox::Ok);
		return;
	}

	// Create the dialog
	DazToUnrealDialog *dlg = new DazToUnrealDialog(mw);

	// If the Accept button was pressed, start the export
	if (dlg->exec() == QDialog::Accepted)
	{

		// Collect the values from the dialog fields
		QString CharacterName = dlg->assetNameEdit->text();
		QString ImportFolder = dlg->intermediateFolderEdit->text();
		QString CharacterFolder = ImportFolder + "\\" + CharacterName + "\\";
		QString CharacterFBX = CharacterFolder + CharacterName + ".fbx";
		QString AssetType = dlg->assetTypeCombo->currentText().replace(" ", "");
		QString MorphString = dlg->GetMorphString();
		int port = dlg->portEdit->text().toInt();
		bool ExportMorphs = dlg->morphsEnabledCheckBox->isChecked();
		bool ExportSubdivisions = dlg->subdivisionEnabledCheckBox->isChecked();

		// Set subdivisions before FBX export
		SubdivisionDialog* subdivisionDialog = SubdivisionDialog::Get(dlg);
		subdivisionDialog->LockSubdivisionProperties(ExportSubdivisions);

		// FBX Export
		DzExportMgr* ExportManager = dzApp->getExportMgr();
		DzExporter* Exporter = ExportManager->findExporterByClassName("DzFbxExporter");

		if (Exporter)
		{
			DzFileIOSettings* FileSettings = new DzFileIOSettings();
			FileSettings->setBoolValue("doSelected", true);
			FileSettings->setBoolValue("doVisible", false);
			if (AssetType == "SkeletalMesh" || AssetType == "StaticMesh")
			{
				FileSettings->setBoolValue("doFigures", true);
				FileSettings->setBoolValue("doProps", true);
			}
			else
			{
				FileSettings->setBoolValue("doFigures", true);
				FileSettings->setBoolValue("doProps", false);
			}
			FileSettings->setBoolValue("doLights", false);
			FileSettings->setBoolValue("doCameras", false);
			FileSettings->setBoolValue("doAnims", true);
			if (AssetType == "SkeletalMesh" && ExportMorphs && MorphString != "")
			{
				FileSettings->setBoolValue("doMorphs", true);
				FileSettings->setStringValue("rules", MorphString);
				//FileSettings->setStringValue("rules", "Weight\n1\n.CTRLVS\n2\nAnything\n0");
				//FileSettings->setStringValue("rules", "CTRLWeight\n1\nFBMZari\n1\n.CTRLVS\n2\nAnything\n0");
			}
			else
			{
				FileSettings->setBoolValue("doMorphs", false);
				FileSettings->setStringValue("rules", "");
			}

			FileSettings->setStringValue("format", dlg->fbxVersionCombo->currentText());
			FileSettings->setIntValue("RunSilent", 1);

			FileSettings->setBoolValue("doEmbed", false);
			FileSettings->setBoolValue("doCopyTextures", false);
			FileSettings->setBoolValue("doDiffuseOpacity", false);
			FileSettings->setBoolValue("doMergeClothing", true);
			FileSettings->setBoolValue("doStaticClothing", false);
			FileSettings->setBoolValue("degradedSkinning", true);
			FileSettings->setBoolValue("degradedScaling", true);
			FileSettings->setBoolValue("doSubD", false);
			FileSettings->setBoolValue("doCollapseUVTiles", false);

			DzNode* Selection = dzScene->getPrimarySelection();
			if (!Selection) return;

			// get the top level node for things like clothing so we don't get dupe material names
			DzNode* Parent = Selection;
			while (Parent->getNodeParent() != NULL)
			{
				Parent = Parent->getNodeParent();
			}

			// rename duplicate material names
			QList<QString> MaterialNames;
			QMap<DzMaterial*, QString> OriginalMaterialNames;
			RenameDuplicateMaterials(Parent, MaterialNames, OriginalMaterialNames);

			QDir dir;
			dir.mkpath(CharacterFolder);
			Exporter->writeFile(CharacterFBX, FileSettings);

			// write the meterial parameters
			{
				QString filename = CharacterFolder + CharacterName + "_Maps.txt";
				QFile file(filename);
				file.open(QIODevice::WriteOnly);
				QTextStream stream(&file);

				stream << "Version, Object, Material, Type, Color, Opacity, File" << endl;
				WriteMaterials(Selection, stream);
				file.close();
			}
			// Change back material names
			UndoRenameDuplicateMaterials(Parent, MaterialNames, OriginalMaterialNames);

			// write the morph renaming
			{
				QString filename = CharacterFolder + CharacterName + "_MorphNames.txt";
				QFile file(filename);
				file.open(QIODevice::WriteOnly);
				QTextStream stream(&file);
				stream << dlg->GetMorphMapping();
				file.close();
			}

			if (ExportSubdivisions)
			{
				QString filename = CharacterFolder + CharacterName + "_Subdivisions.txt";
				subdivisionDialog->WriteSubdivisionFile(filename);
			}
		}

		// Send a message to Unreal telling it to start an import
		QUdpSocket* sendSocket = new QUdpSocket(this);
		QHostAddress* sendAddress = new QHostAddress("127.0.0.1");

		sendSocket->connectToHost(*sendAddress, port);

		QString Message = "ASSETTYPE=" + AssetType;
		Message += "|FBXFILE=" + CharacterFBX;
		Message += "|ASSETNAME=" + CharacterName;
		Message += "|IMPORTFOLDER=" + CharacterFolder;
		sendSocket->write(Message.toUtf8());

		// Write the message out so we can manually import
		QString DTUfilename = CharacterFolder + CharacterName + ".dtu";
		QFile DTUfile(DTUfilename);
		DTUfile.open(QIODevice::WriteOnly);
		QTextStream stream(&DTUfile);
		stream << Message << endl;
		DTUfile.close();

		return;
	}
}

// Write out all the surface properties
void DazToUnrealAction::WriteMaterials(DzNode* Node, QTextStream& Stream)
{
	DzObject* Object = Node->getObject();
	DzShape* Shape = Object ? Object->getCurrentShape() : NULL;

	if (Shape)
	{
		for (int i = 0; i < Shape->getNumMaterials(); i++)
		{
			DzMaterial* Material = Shape->getMaterial(i);
			if (Material)
			{	
				DzPresentation* presentation = Node->getPresentation();
				if (presentation)
				{
					const QString presentationType = presentation->getType();
					Stream << "2, " << Node->getLabel() << ", " << Material->getName() << ", " << Material->getMaterialName() << ", " << "Asset Type" << ", " << presentationType << ", " << "String" << ", " << "" << endl;
				}

				for (int propertyIndex = 0; propertyIndex < Material->getNumProperties(); propertyIndex++)
				{
					DzProperty* Property = Material->getProperty(propertyIndex);
					DzImageProperty* ImageProperty = qobject_cast<DzImageProperty*>(Property);
					if (ImageProperty)
					{
						QString Name = Property->getName();
						QString TextureName = "";
						
						if (ImageProperty->getValue())
						{
							TextureName = ImageProperty->getValue()->getFilename();
						}
						Stream << "2, " << Node->getLabel() << ", " << Material->getName() << ", " << Material->getMaterialName() << ", " << Name << ", " << Material->getDiffuseColor().name() << ", " << "Texture" << ", " << TextureName << endl;
					}
					DzColorProperty* ColorProperty = qobject_cast<DzColorProperty*>(Property);
					if (ColorProperty)
					{
						QString Name = Property->getName();
						QString TextureName = "";

						if (ColorProperty->getMapValue())
						{
							TextureName = ColorProperty->getMapValue()->getFilename();

						}
						Stream << "2, " << Node->getLabel() << ", " << Material->getName() << ", " << Material->getMaterialName() << ", " << Name << ", " << ColorProperty->getColorValue().name() << ", " << "Color" << ", " << TextureName << endl;
						continue;
					}

					DzNumericProperty* NumericProperty = qobject_cast<DzNumericProperty*>(Property);
					if (NumericProperty)
					{
						QString Name = Property->getName();
						QString TextureName = "";

						if (NumericProperty->getMapValue())
						{
							TextureName = NumericProperty->getMapValue()->getFilename();
							
						}
						Stream << "2, " << Node->getLabel() << ", " << Material->getName() << ", " << Material->getMaterialName() << ", " << Name << ", " << NumericProperty->getDoubleValue() << ", " << "Double" << ", " << TextureName << endl;
					}
				}
			}
		}
	}

	DzNodeListIterator Iterator = Node->nodeChildrenIterator();
	while (Iterator.hasNext())
	{
		DzNode* Child = Iterator.next();
		WriteMaterials(Child, Stream);
	}
}

// If there are duplicate material names, save off the original and rename one
void DazToUnrealAction::RenameDuplicateMaterials(DzNode* Node, QList<QString>& MaterialNames, QMap<DzMaterial*, QString>& OriginalMaterialNames)
{
	DzObject* Object = Node->getObject();
	DzShape* Shape = Object ? Object->getCurrentShape() : NULL;


	if (Shape)
	{
		for (int i = 0; i < Shape->getNumMaterials(); i++)
		{
			DzMaterial* Material = Shape->getMaterial(i);
			if (Material)
			{
				OriginalMaterialNames.insert(Material, Material->getName());
				while (MaterialNames.contains(Material->getName()))
				{
					Material->setName(Material->getName() + "_1");
				}
				MaterialNames.append(Material->getName());
			}
		}
	}
	DzNodeListIterator Iterator = Node->nodeChildrenIterator();
	while (Iterator.hasNext())
	{
		DzNode* Child = Iterator.next();
		RenameDuplicateMaterials(Child, MaterialNames, OriginalMaterialNames);
	}
}

// Restore the original material names
void DazToUnrealAction::UndoRenameDuplicateMaterials(DzNode* Node, QList<QString>& MaterialNames, QMap<DzMaterial*, QString>& OriginalMaterialNames)
{
	QMap<DzMaterial*, QString>::iterator iter;
	for (iter = OriginalMaterialNames.begin(); iter != OriginalMaterialNames.end(); ++iter)
	{
		iter.key()->setName(iter.value());
	}
}
