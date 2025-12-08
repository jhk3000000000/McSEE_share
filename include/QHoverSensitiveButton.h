#pragma once

#ifndef QHOVERSENSITIVEBUTTON_H
#define QHOVERSENSITIVEBUTTON_H

#include "pch.h"
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/QMainWindow>
#include "ETInteractorStyle.h"
#include "ETInteractorStyleTrackballActor.h"
#include "ETQVTKWidget.h"
#include "FunctionPanelRight.h"
#include "ETHuman3D.h"
#include "ETHuman3DApp.h"

#include "Util.h"
#include <array>


class QHoverSensitiveButton : public QPushButton
{
public:
	explicit QHoverSensitiveButton(QWidget *parent = 0);

protected:
	void hoverEnter(QHoverEvent *event);
	void hoverLeave(QHoverEvent *event);
	void hoverMove(QHoverEvent *event);
	bool event(QEvent *event);

signals:

public slots:
};

#endif // QHOVERSENSITIVEBUTTON_H