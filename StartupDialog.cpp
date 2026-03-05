//
// Created by Tiziano on 05.03.2026.
//

#include "StartupDialog.h"

#include <QRadioButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QButtonGroup>

StartupDialog::StartupDialog(QWidget *parent) : QDialog(parent) {
	setFixedSize(200, 125);
	setWindowTitle("Seleziona modalità di avvio");

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel("Selezionare il tipo di griglia"));

	radioGroup = new QButtonGroup(this);

	QRadioButton *mode1 = new QRadioButton("3 x 3");
	QRadioButton *mode2 = new QRadioButton("4 x 4");

	radioGroup->addButton(mode1, 1);
	radioGroup->addButton(mode2, 2);

	mode1->setChecked(true);

	layout->addWidget(mode1);
	layout->addWidget(mode2);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	layout->addWidget(buttonBox);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int StartupDialog::getSelectedMode() const {
	return radioGroup->checkedId();
}
