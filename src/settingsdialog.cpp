#include <QApplication>
#include <QLocale>
#include <QMessageBox>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    _lang(""), _langChanged(false), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    for (int i=0; i<ccLast; i++)
         validColumnNames << contactColumnHeaders[i];
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::readConfig()
{
    // Language
    _lang = settings.value("General/Language", "Unknown").toString();
    if (_lang=="Unknown") {
        _lang = "English"; // TODO use system language
    }
    ui->cbLanguage->setCurrentIndex(ui->cbLanguage->findText(_lang));
    // Locale
    ui->leDateFormat->setText(settings.value("Locale/DateFormat", QLocale::system().dateFormat()).toString());
    ui->leTimeFormat->setText(settings.value("Locale/TimeFormat", QLocale::system().timeFormat()).toString());
    ui->cbUseSystemDateTimeFormat->setChecked(settings.value("Locale/UseSystemDAteTimeFormat", true).toBool());
    on_cbUseSystemDateTimeFormat_clicked(ui->cbUseSystemDateTimeFormat->isChecked());
    // Misc
    ui->cbOpenLastFilesAtStartup->setChecked(settings.value("General/OpenLastFilesAtStartup", true).toBool());
    // Column view
    int visibleColumnCount = settings.value("VisibleColumns/Count", 0).toInt();
    for (int i=0; i<visibleColumnCount; i++) { // Fill visible columns list
        QString columnCandidate = settings.value(QString("VisibleColumns/Column%1").arg(i+1)).toString();
        if (validColumnNames.contains(columnCandidate))
            ui->lwVisibleColumns->addItem(columnCandidate);
    }
    if (ui->lwVisibleColumns->count()==0) { // if list is empty, set default
        ui->lwVisibleColumns->addItem(contactColumnHeaders[ccLastName]);
        ui->lwVisibleColumns->addItem(contactColumnHeaders[ccFirstName]);
        ui->lwVisibleColumns->addItem(contactColumnHeaders[ccPhone]);
    }
    for (int i=0; i<validColumnNames.count(); i++) // Fill available columns list
        if (ui->lwVisibleColumns->findItems(validColumnNames[i], Qt::MatchCaseSensitive).isEmpty())
            ui->lwAvailableColumns->addItem(validColumnNames[i]);
    return true;
}

bool SettingsDialog::writeConfig()
{
    // Language
    QString newLang = ui->cbLanguage->currentText();
    settings.setValue("General/Language", newLang);
    if (newLang!=_lang) {
        _langChanged = true;
        _lang = newLang;
    }
    // Locale
    settings.setValue("Locale/DateFormat", ui->leDateFormat->text());
    settings.setValue("Locale/TimeFormat", ui->leTimeFormat->text());
    settings.setValue("Locale/UseSystemDAteTimeFormat", ui->cbUseSystemDateTimeFormat->isChecked());
    // Misc
    settings.setValue("General/OpenLastFilesAtStartup", ui->cbOpenLastFilesAtStartup->isChecked());
    // Column view
    settings.setValue("VisibleColumns/Count", ui->lwVisibleColumns->count());
    for (int i=0; i<ui->lwVisibleColumns->count(); i++)
        settings.setValue(QString("VisibleColumns/Column%1").arg(i+1),
                          ui->lwVisibleColumns->item(i)->text());
    return true;
}

QString SettingsDialog::lastPath()
{
    QString defaultDir =
#if QT_VERSION >= 0x050000
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
        QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation);
#endif
    return settings.value("General/LastFile", defaultDir).toString();
}

void SettingsDialog::setLastPath(const QString &path)
{
    settings.setValue("General/LastFile", path);
}

bool SettingsDialog::showTwoPanels()
{
    return settings.value("General/ShowTwoPanels", true).toBool();
}

void SettingsDialog::setShowTwoPanels(bool value)
{
    settings.setValue("General/ShowTwoPanels", value);
}

bool SettingsDialog::openLastFilesAtStartup()
{
    return ui->cbOpenLastFilesAtStartup->isChecked();
}

bool SettingsDialog::sortingEnabled()
{
    return settings.value("General/SortingEnabled", false).toBool();
}

void SettingsDialog::setSortingEnabled(bool value)
{
    settings.setValue("General/SortingEnabled", value);

}

ContactColumnList SettingsDialog::columnNames()
{
    ContactColumnList res;
    for (int i=0; i<ui->lwVisibleColumns->count(); i++)
        res << (ContactColumn)validColumnNames.indexOf(ui->lwVisibleColumns->item(i)->text());
    return res;
}

QString SettingsDialog::lang()
{
    return _lang;
}

bool SettingsDialog::langChanged()
{
    return _langChanged;
}

QString SettingsDialog::dateFormat()
{
    if (ui->cbUseSystemDateTimeFormat->isChecked())
        return QLocale::system().dateFormat();
    else
        return ui->leDateFormat->text();
}

QString SettingsDialog::timeFormat()
{
    if (ui->cbUseSystemDateTimeFormat->isChecked())
        return QLocale::system().timeFormat();
    else
        return ui->leTimeFormat->text();
}

void SettingsDialog::on_btnAddCol_clicked()
{
    foreach (QListWidgetItem* item, ui->lwAvailableColumns->selectedItems()) {
        ui->lwVisibleColumns->addItem(item->text());
        delete item;
    }
}

void SettingsDialog::on_btnDelCol_clicked()
{
    if (ui->lwVisibleColumns->selectedItems().count()>=ui->lwVisibleColumns->count()) {
        QMessageBox::critical(0, S_ERROR, tr("List must contain at least one visible column"));
    }
    else
    foreach (QListWidgetItem* item, ui->lwVisibleColumns->selectedItems()) {
        ui->lwAvailableColumns->addItem(item->text());
        delete item;
    }
}

void SettingsDialog::on_btnUpCol_clicked()
{
    for (int i=1; i<ui->lwVisibleColumns->count(); i++) {
        QListWidgetItem* item = ui->lwVisibleColumns->item(i);
        if (item->isSelected()) {
            QString colName = item->text();
            delete item;
            ui->lwVisibleColumns->insertItem(i-1, colName);
            ui->lwVisibleColumns->item(i-1)->setSelected(true);
        }
    }
}

void SettingsDialog::on_btnDownCol_clicked()
{
    for (int i=ui->lwVisibleColumns->count()-2; i>=0; i--) {
        QListWidgetItem* item = ui->lwVisibleColumns->item(i);
        if (item->isSelected()) {
            QString colName = item->text();
            delete item;
            ui->lwVisibleColumns->insertItem(i+1, colName);
            ui->lwVisibleColumns->item(i+1)->setSelected(true);
        }
    }
}


void SettingsDialog::on_cbUseSystemDateTimeFormat_clicked(bool checked)
{
    ui->leDateFormat->setEnabled(!checked);
    ui->leTimeFormat->setEnabled(!checked);
}
