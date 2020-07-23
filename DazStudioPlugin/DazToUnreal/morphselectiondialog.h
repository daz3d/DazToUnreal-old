// Copyright (C) 2018-2020 David Vodhanel All Rights Reserved.

#pragma once
#include "dzbasicdialog.h"
#include <QtGui/qlineedit.h>
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>
#include "dznode.h"

class QListWidget;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QComboBox;

struct MorphInfo {
	QString Name = QString();
	QString Label = QString();
	QString Type = QString();
	QString Path = QString();

	inline bool operator==(MorphInfo other)
	{
		if (Name == other.Name)
		{
			return true;
		}
		return false;
	}
};

class MorphSelectionDialog : public DzBasicDialog {
	Q_OBJECT
public:

	MorphSelectionDialog(QWidget *parent);
	virtual ~MorphSelectionDialog() {}

	// Setup the dialog
	void PrepareDialog();

	// Singleton access
	static MorphSelectionDialog* Get(QWidget* Parent)
	{
		if (singleton == nullptr)
		{
			singleton = new MorphSelectionDialog(Parent);
		}
		singleton->PrepareDialog();
		return singleton;
	}
	
	// Get the morph string in the format for the Daz FBX Export
	QString GetMorphString();

	// Get the morph string in the format used for presets
	QString GetMorphCSVString();

	// Get the morph string in an internal name = friendly name format
	// Used to rename them to the friendly name in Unreal
	QString GetMorphRenaming();


public slots:
	void FilterChanged(const QString& filter);
	void ItemSelectionChanged();
	void HandleAddMorphsButton();
	void HandleRemoveMorphsButton();
	void HandleSavePreset();
	void HandlePresetChanged(const QString& presetName);
	void HandleArmJCMMorphsButton();
	void HandleLegJCMMorphsButton();
	void HandleTorsoJCMMorphsButton();

private:

	// Refresh the list of possible presets from disk
	void RefreshPresetsCombo();

	// Recursive function for finding all morphs for a node
	QStringList GetAvailableMorphs(DzNode* Node);

	void UpdateMorphsTree();

	// Returns the tree node for the morph name (with path)
	// Builds out the structure of the tree as needed.
	QTreeWidgetItem* FindTreeItem(QTreeWidgetItem* parent, QString name);

	void SavePresetFile(QString filePath);

	// Updates selectedInTree to have all the morphs for the nodes
	// selected in the left tree
	void SelectMorphsInNode(QTreeWidgetItem* item);

	// Rebuild the right box that lists all the morphs that will export.
	void RefreshExportMorphList();

	// Morphs currently selected in the left tree box
	QList<MorphInfo> selectedInTree;

	// List of morphs moved to the export box
	QList<MorphInfo> morphsToExport;

	// Store off the presetsFolder path at dialog setup
	QString presetsFolder;

	static MorphSelectionDialog* singleton;

	// A list of all found morphs.
	QStringList morphList;

	// Mapping of morph name to label
	QMap<QString, QString> morphNameMapping;

	// Mapping of morph name to MorphInfo
	//TODO: morphNameMapping (and others) should be replaced by this
	QMap<QString, MorphInfo> morphs;

	// List of morphs (recursive) under each tree node
	// For convenience populating the middle box.
	QMap<QTreeWidgetItem*, QList<MorphInfo>> morphsForNode;

	// Force the size of the dialog
	QSize minimumSizeHint() const override;

	// Widgets the dialog will access after construction
	QListWidget* morphListWidget = NULL;
	QListWidget* morphExportListWidget = NULL;
	QTreeWidget* morphTreeWidget = NULL;
	QLineEdit* filterEdit = NULL;
	QComboBox* presetCombo = NULL;

	QTreeWidgetItem* fullBodyMorphTreeItem = NULL;
	QTreeWidgetItem* charactersTreeItem = NULL;
};
