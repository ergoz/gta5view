/*****************************************************************************
* gta5sync GRAND THEFT AUTO V SYNC
* Copyright (C) 2016 Syping
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"
#include "AppEnv.h"
#include "config.h"
#include <QMessageBox>
#include <QStringList>
#include <QLocale>
#include <QString>
#include <QDebug>
#include <QList>
#include <QDir>

OptionsDialog::OptionsDialog(ProfileDatabase *profileDB, QWidget *parent) :
    QDialog(parent), profileDB(profileDB),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    contentMode = 0;
    settings = new QSettings(GTA5SYNC_APPVENDOR, GTA5SYNC_APPSTR);

    setupTreeWidget();
    setupLanguageBox();
    setupRadioButtons();
    setupDefaultProfile();
}

OptionsDialog::~OptionsDialog()
{
    delete settings;
    foreach(QTreeWidgetItem *playerItem, playerItems)
    {
        delete playerItem;
    }
    delete ui;
}

void OptionsDialog::setupTreeWidget()
{
    foreach(const QString &playerIDStr, profileDB->getPlayers())
    {
        bool ok;
        int playerID = playerIDStr.toInt(&ok);
        if (ok)
        {
            QString playerName = profileDB->getPlayerName(playerID);

            QStringList playerTreeViewList;
            playerTreeViewList << playerIDStr;
            playerTreeViewList << playerName;

            QTreeWidgetItem *playerItem = new QTreeWidgetItem(playerTreeViewList);
            ui->twPlayers->addTopLevelItem(playerItem);
            playerItems.append(playerItem);
        }
    }
    ui->twPlayers->sortItems(1, Qt::AscendingOrder);
}

void OptionsDialog::setupLanguageBox()
{
    settings->beginGroup("Interface");
    currentLanguage = settings->value("Language","System").toString();
    settings->endGroup();

    QStringList langList = QLocale::system().name().split("_");
    if (langList.length() > 0)
    {
        QString cbSysStr = tr("%1 (%2 if available)", "System like PC System = %1, System Language like Deutsch = %2").arg(tr("System",
        "System like PC System"), QLocale::languageToString(QLocale(langList.at(0)).language()));
        ui->cbLanguage->addItem(cbSysStr, "System");
    }

    QString cbEngStr = "English (English) [en]";
    ui->cbLanguage->addItem(QIcon::fromTheme("flag-us"), cbEngStr, "en");
    if (currentLanguage == "en")
    {
#if QT_VERSION >= 0x050000
        ui->cbLanguage->setCurrentText(cbEngStr);
#else
        int indexOfEnglish = ui->cbLanguage->findText(cbEngStr);
        ui->cbLanguage->setCurrentIndex(indexOfEnglish);
#endif
    }

    QDir langDir;
    langDir.setNameFilters(QStringList("gta5sync_*.qm"));
    langDir.setPath(AppEnv::getLangFolder());
    QStringList langFiles;
    langFiles << langDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort);
    langDir.setPath(":/tr");
    langFiles << langDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort);
    langFiles.removeDuplicates();

    foreach(const QString &langFile, langFiles)
    {
        QString lang = langFile;
        lang.remove("gta5sync_");
        lang.remove(".qm");

        QLocale langLocale(lang);
        QString languageNameInternational = QLocale::languageToString(langLocale.language());
        QString languageNameNative = langLocale.nativeLanguageName();

        QString cbLangStr = languageNameNative + " (" + languageNameInternational + ") [" + lang + "]";
        QString langIconStr = "flag-" + lang;

        ui->cbLanguage->addItem(QIcon::fromTheme(langIconStr), cbLangStr, lang);
        if (currentLanguage == lang)
        {
#if QT_VERSION >= 0x050000
            ui->cbLanguage->setCurrentText(cbLangStr);
#else
            int indexOfLang = ui->cbLanguage->findText(cbLangStr);
            ui->cbLanguage->setCurrentIndex(indexOfLang);
#endif
        }
    }
}

void OptionsDialog::setupRadioButtons()
{
    bool contentModeOk;
    settings->beginGroup("Profile");
    contentMode = settings->value("ContentMode", 0).toInt(&contentModeOk);
    settings->endGroup();

    if (contentModeOk)
    {
        if (contentMode == 0)
        {
            ui->rbOpenWithSC->setChecked(true);
        }
        else if (contentMode == 1)
        {
            ui->rbOpenWithDC->setChecked(true);
        }
        else if (contentMode == 2)
        {
            ui->rbSelectWithSC->setChecked(true);
        }
    }
}

void OptionsDialog::on_cmdOK_clicked()
{
    applySettings();
    close();
}

void OptionsDialog::applySettings()
{
    settings->beginGroup("Interface");
#if QT_VERSION >= 0x050000
    settings->setValue("Language", ui->cbLanguage->currentData());
#else
    settings->setValue("Language", ui->cbLanguage->itemData(ui->cbLanguage->currentIndex()));
#endif
    settings->endGroup();

    settings->beginGroup("Profile");
    int newContentMode = 0;
    if (ui->rbOpenWithSC->isChecked())
    {
        newContentMode = 0;
    }
    else if (ui->rbOpenWithDC->isChecked())
    {
        newContentMode = 1;
    }
    else if (ui->rbSelectWithSC->isChecked())
    {
        newContentMode = 2;
    }
    settings->setValue("ContentMode", newContentMode);
#if QT_VERSION >= 0x050000
    settings->setValue("Default", ui->cbProfiles->currentData());
#else
    settings->setValue("Default", ui->cbProfiles->itemData(ui->cbProfiles->currentIndex()));
#endif
    settings->endGroup();

#if QT_VERSION >= 0x050000
    emit settingsApplied(newContentMode, ui->cbLanguage->currentData().toString());
#else
    emit settingsApplied(newContentMode, ui->cbLanguage->itemData(ui->cbLanguage->currentIndex()).toString());
#endif

#if QT_VERSION >= 0x050000
    bool languageChanged = ui->cbLanguage->currentData().toString() != currentLanguage;
#else
    bool languageChanged = ui->cbLanguage->itemData(ui->cbLanguage->currentIndex()).toString() != currentLanguage;
#endif
    if (languageChanged)
    {
        QMessageBox::information(this, tr("%1", "%1").arg(GTA5SYNC_APPSTR), tr("The language change will take effect after you restart %1.").arg(GTA5SYNC_APPSTR));
    }
}

void OptionsDialog::setupDefaultProfile()
{
    settings->beginGroup("Profile");
    defaultProfile = settings->value("Default", "").toString();
    settings->endGroup();

    QString cbNoneStr = tr("No Profile", "No Profile, as default");
    ui->cbProfiles->addItem(cbNoneStr, "");
}

void OptionsDialog::commitProfiles(QStringList profiles)
{
    foreach(const QString &profile, profiles)
    {
        ui->cbProfiles->addItem(tr("Profile: %1").arg(profile), profile);
        if (defaultProfile == profile)
        {
#if QT_VERSION >= 0x050000
            ui->cbProfiles->setCurrentText(tr("Profile: %1").arg(profile));
#else
            int indexOfProfile = ui->cbProfiles->findText(tr("Profile: %1").arg(profile));
            ui->cbProfiles->setCurrentIndex(indexOfProfile);
#endif
        }
    }
}
