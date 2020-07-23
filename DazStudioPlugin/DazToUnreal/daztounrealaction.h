// Copyright (C) 2018-2020 David Vodhanel All Rights Reserved.

#pragma once
#include "dzaction.h"
#include "dznode.h"
#include "QtCore/qfile.h"
#include "QtCore/qtextstream.h"
class DazToUnrealAction : public DzEditAction {
	Q_OBJECT
public:

	DazToUnrealAction();

protected:

	virtual void	executeAction();

	// Writes the _Maps.txt file with all the surface parameters
	void WriteMaterials(DzNode* Node, QTextStream& Stream);

	// Need to temporarily rename surfaces if there is a name collision
	void RenameDuplicateMaterials(DzNode* Node, QList<QString>& MaterialNames, QMap<DzMaterial*, QString>& OriginalMaterialNames);
	void UndoRenameDuplicateMaterials(DzNode* Node, QList<QString>& MaterialNames, QMap<DzMaterial*, QString>& OriginalMaterialNames);

};