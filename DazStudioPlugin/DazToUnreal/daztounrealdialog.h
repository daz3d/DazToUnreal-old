#pragma once
#include "dzbasicdialog.h"
//#include <QtGui/qlineedit.h>
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>

class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QGroupBox;

class DazToUnrealDialog : public DzBasicDialog {
	Q_OBJECT
public:

	/** Constructor **/
	DazToUnrealDialog(QWidget *parent);

	/** Destructor **/
	virtual ~DazToUnrealDialog() {}

	QLineEdit* assetNameEdit = NULL;
	QLineEdit* projectEdit = NULL;
	QPushButton* projectButton = NULL;
	QComboBox* assetTypeCombo = NULL;
	QLineEdit* portEdit = NULL;
	QLineEdit* intermediateFolderEdit = NULL;
	QPushButton* intermediateFolderButton = NULL;
	QPushButton* morphsButton = NULL;
	QCheckBox* morphsEnabledCheckBox = NULL;
	QPushButton* subdivisionButton = NULL;
	QCheckBox* subdivisionEnabledCheckBox = NULL;
	QGroupBox* advancedSettingsGroupBox = NULL;
	QComboBox* fbxVersionCombo = NULL;
	QCheckBox* showFbxDialogCheckBox = NULL;

	// Pass so the DazTRoUnrealAction can access it from the morph dialog
	QString GetMorphString();

	// Pass so the DazTRoUnrealAction can access it from the morph dialog
	QString GetMorphMapping() { return morphMapping; }

	void Accepted();
private slots:
	void HandleSelectIntermediateFolderButton();
	void HandlePortChanged(const QString& port);
	void HandleChooseMorphsButton();
	void HandleMorphsCheckBoxChange(int state);
	void HandleChooseSubdivisionsButton();
	void HandleSubdivisionCheckBoxChange(int state);
	void HandleFBXVersionChange(const QString& fbxVersion);
	void HandleShowFbxDialogCheckBoxChange(int state);
private:
	QSettings* settings;

	// These are clumsy leftovers from before the dialog were singletons
	QString morphString;
	QString morphMapping;
};
