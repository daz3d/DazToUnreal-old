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
class QGridLayout;


class SubdivisionDialog : public DzBasicDialog {
	Q_OBJECT
public:

	/** Constructor **/
	SubdivisionDialog(QWidget *parent);

	void PrepareDialog();

	/** Destructor **/
	virtual ~SubdivisionDialog() {}

	static SubdivisionDialog* Get(QWidget* Parent)
	{
		if (singleton == nullptr)
		{
			singleton = new SubdivisionDialog(Parent);
		}
		singleton->PrepareDialog();
		return singleton;
	}

	QGridLayout* subdivisionItemsGrid = NULL;

	void LockSubdivisionProperties(bool subdivisionEnabled);
	void WriteSubdivisionFile(QString path);
	DzNode* FindObject(DzNode* Node, QString Name);

public slots:
	void HandleSubdivisionLevelChanged(const QString& text);

private:
	void CreateList(DzNode* Node);

	void SavePresetFile(QString filePath);

	QSize minimumSizeHint() const override;

	QString presetsFolder;

	QList<QComboBox*> SubdivisionCombos;

	QMap<QString, int> SubdivisionLevels;

	static SubdivisionDialog* singleton;
};
