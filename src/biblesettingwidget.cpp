/***************************************************************************
//
//    softProjector - an open source media projection software
//    Copyright (C) 2014  Vladislav Kobzar, Matvey Adzhigirey and Ilya Spivakov
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation version 3 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
***************************************************************************/

#include "biblesettingwidget.h"
#include "ui_biblesettingwidget.h"

BibleSettingWidget::BibleSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BibleSettingWidget)
{
    ui->setupUi(this);
    isLoading = false;
}

BibleSettingWidget::~BibleSettingWidget()
{
    delete ui;
}

void BibleSettingWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
     switch ( e->type() ) {
     case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
     default:
         break;
     }
}

void BibleSettingWidget::setSettings(BibleSettings &settings, BibleSettings &settings2)
{
    isLoading = true;
    mySettings = settings;
    mySettings2 = settings2;

    loadSettings();
    mySettings.resetChangeHandles();
    mySettings2.resetChangeHandles();
    isLoading = false;
}

void BibleSettingWidget::getSettings(BibleSettings &settings, BibleSettings &settings2)
{
    settings = mySettings;
    settings2 = mySettings2;
    mySettings.resetChangeHandles();
    mySettings2.resetChangeHandles();
}

void BibleSettingWidget::setBibleVersions(BibleVersionSettings &bver, BibleVersionSettings &bver2)
{
    bversion = bver;
    bversion2 = bver2;
    loadBibleVersions();
}

void BibleSettingWidget::getBibleVersions(BibleVersionSettings &bver, BibleVersionSettings &bver2)
{
    int pbx = ui->comboBoxPrimaryBible->currentIndex();
    int sbx = ui->comboBoxSecondaryBible->currentIndex();
    int tbx = ui->comboBoxTrinaryBible->currentIndex();
    int obx = ui->comboBoxOperatorBible->currentIndex();

    // Get Bible version settings
    if(pbx != -1)
    {
        // Primary
        bversion.primaryBible = bible_id_list.at(pbx);
        // Secondary
        if(sbx <=0)
            bversion.secondaryBible = "none";
        else
            bversion.secondaryBible = secondary_id_list.at(sbx-1);
        // Trinary
        if(tbx <=0)
            bversion.trinaryBible = "none";
        else
            bversion.trinaryBible = trinary_id_list.at(tbx-1);
        // Operatror
        if(obx <=0)
            bversion.operatorBible = "same";
        else
            bversion.operatorBible = operator_id_list.at(obx-1);
    }
    else
    {
        // There are no bibles in the database
        bversion.primaryBible = "none";
        bversion.secondaryBible = "none";
        bversion.trinaryBible = "none";
        bversion.operatorBible = "same";
    }

    // Get Bible versions for secondary screen
    pbx = ui->comboBoxPrimaryBible2->currentIndex();
    sbx = ui->comboBoxSecondaryBible2->currentIndex();
    tbx = ui->comboBoxTrinaryBible2->currentIndex();
    if(pbx != -1)
    {
        // Primary
        bversion2.primaryBible = bible_id_list.at(pbx);
        // Secondary
        if(sbx <=0)
            bversion2.secondaryBible = "none";
        else
            bversion2.secondaryBible = secondary_id_list2.at(sbx-1);
        // Trinary
        if(tbx <=0)
            bversion2.trinaryBible = "none";
        else
            bversion2.trinaryBible = trinary_id_list2.at(tbx-1);
    }
    else
    {
        // There are no bibles in the database
        bversion2.primaryBible = "none";
        bversion2.secondaryBible = "none";
        bversion2.trinaryBible = "none";
        bversion2.operatorBible = "same";
    }

    bver = bversion;
    bver2 = bversion2;
}

void BibleSettingWidget::loadSettings()
{
    QPalette p;

    // Text Font
    ui->checkBoxCommonFont->setChecked(mySettings.isNotCommonFont);
    on_checkBoxCommonFont_stateChanged(mySettings.isNotCommonFont);
    ui->labelFont->setText(getFontText(mySettings.textFont));
    ui->checkBoxCommonFont2->setChecked(mySettings2.isNotCommonFont);
    on_checkBoxCommonFont2_stateChanged(mySettings2.isNotCommonFont);
    ui->labelFont2->setText(getFontText(mySettings2.textFont));

    // Text Color
    ui->checkBoxCommonColor->setChecked(mySettings.isNotCommonColor);
    on_checkBoxCommonColor_stateChanged(mySettings.isNotCommonColor);
    p.setColor(QPalette::Base,mySettings.textColor);
    ui->graphicViewTextColor->setPalette(p);
    ui->checkBoxCommonColor2->setChecked(mySettings2.isNotCommonColor);
    on_checkBoxCommonColor2_stateChanged(mySettings2.isNotCommonColor);
    p.setColor(QPalette::Base,mySettings2.textColor);
    ui->graphicViewTextColor2->setPalette(p);

    // Text Shadow Color
    p.setColor(QPalette::Base,mySettings.textShadowColor);
    ui->graphicViewShadowColor->setPalette(p);
    p.setColor(QPalette::Base,mySettings2.textShadowColor);
    ui->graphicViewShadowColor2->setPalette(p);

    // Alingment
    ui->comboBoxVerticalAling->setCurrentIndex(mySettings.textAlingmentV);
    ui->comboBoxHorizontalAling->setCurrentIndex(mySettings.textAlingmentH);
    ui->comboBoxVerticalAling2->setCurrentIndex(mySettings2.textAlingmentV);
    ui->comboBoxHorizontalAling2->setCurrentIndex(mySettings2.textAlingmentH);

    // Caption Font
    ui->checkBoxSameFont->setChecked(mySettings.isNotSameFont);
    on_checkBoxSameFont_stateChanged(mySettings.isNotSameFont);
    ui->labelFontCaption->setText(getFontText(mySettings.captionFont));
    ui->checkBoxSameFont2->setChecked(mySettings2.isNotSameFont);
    on_checkBoxSameFont2_stateChanged(mySettings2.isNotSameFont);
    ui->labelFontCaption2->setText(getFontText(mySettings2.captionFont));

    // Caption Color
    ui->checkBoxSameColor->setChecked(mySettings.isNotSameColor);
    on_checkBoxSameColor_stateChanged(mySettings.isNotSameColor);
    p.setColor(QPalette::Base,mySettings.captionColor);
    ui->graphicViewCaptionColor->setPalette(p);
    ui->checkBoxSameColor2->setChecked(mySettings2.isNotSameColor);
    on_checkBoxSameColor2_stateChanged(mySettings2.isNotSameColor);
    p.setColor(QPalette::Base,mySettings2.captionColor);
    ui->graphicViewCaptionColor2->setPalette(p);

    // Caption Shadow Color
    p.setColor(QPalette::Base,mySettings.captionShadowColor);
    ui->graphicViewCaptionShadowColor->setPalette(p);
    p.setColor(QPalette::Base,mySettings2.captionShadowColor);
    ui->graphicViewCaptionShadowColor2->setPalette(p);

    // Caption Alingment
    ui->comboBoxCaptionPosition->setCurrentIndex(mySettings.captionPosition);
    ui->comboBoxCaptionPosition2->setCurrentIndex(mySettings2.captionPosition);
    ui->comboBoxCaptionAlign->setCurrentIndex(mySettings.captionAlingment);
    ui->comboBoxCaptionAlign2->setCurrentIndex(mySettings2.captionAlingment);

    // Transition
    ui->comboBoxTransitionType->setCurrentIndex(mySettings.transitionType + 1);
    ui->comboBoxTransitionType2->setCurrentIndex(mySettings2.transitionType + 1);

    // Effects
    ui->comboBoxTextEffects->setCurrentIndex(mySettings.effectsType + 1);
    ui->comboBoxTextEffects2->setCurrentIndex(mySettings2.effectsType + 1);

    // Background
    ui->comboBoxBackgoundType->setCurrentIndex(mySettings.backgroundType + 1);
    on_comboBoxBackgoundType_currentIndexChanged(mySettings.backgroundType + 1);
    p.setColor(QPalette::Base,mySettings.backgroundColor);
    ui->graphicsViewBackgroundColor->setPalette(p);
    if(mySettings.backgroundType == 1)
        ui->lineEditBackPath->setText(mySettings.backgroundName);
    else if(mySettings.backgroundType == 2)
        ui->lineEditBackPath->setText(mySettings.backgroundVideoPath);

    ui->comboBoxBackgoundType2->setCurrentIndex(mySettings2.backgroundType + 1);
    on_comboBoxBackgoundType2_currentIndexChanged(mySettings2.backgroundType + 1);
    p.setColor(QPalette::Base,mySettings2.backgroundColor);
    ui->graphicsViewBackgroundColor2->setPalette(p);
    if(mySettings2.backgroundType == 1)
        ui->lineEditBackPath2->setText(mySettings2.backgroundName);
    else if(mySettings2.backgroundType == 2)
        ui->lineEditBackPath2->setText(mySettings2.backgroundVideoPath);

    // Abbriviations
    ui->checkBoxAbbiviations->setChecked(mySettings.useAbbriviation);
    ui->checkBoxAbbiviations2->setChecked(mySettings2.useAbbriviation);

    // Screen Use
    ui->checkBoxCommonLayout->setChecked(mySettings.isNotCommonLayout);
    on_checkBoxCommonLayout_stateChanged(mySettings.isNotCommonLayout);
    ui->spinBoxMaxScreen->setValue(mySettings.screenUse);
    ui->comboBoxScreenPosition->setCurrentIndex(mySettings.screenPosition);
    ui->checkBoxCommonLayout2->setChecked(mySettings2.isNotCommonLayout);
    on_checkBoxCommonLayout2_stateChanged(mySettings2.isNotCommonLayout);
    ui->spinBoxMaxScreen2->setValue(mySettings2.screenUse);
    ui->comboBoxScreenPosition2->setCurrentIndex(mySettings2.screenPosition);

    // Secondary Screen
    ui->checkBoxDisp2->setChecked(mySettings.useSameForDisp2);
    on_checkBoxDisp2_stateChanged(mySettings.useSameForDisp2);
}

void BibleSettingWidget::loadBibleVersions()
{
    // Clear items if exitst
    bibles.clear();
    bible_id_list.clear();

    // Get Bibles that exist in the database
    QSqlQuery sq;
    sq.exec("SELECT bible_name, id FROM BibleVersions");
    while(sq.next())
    {
        bibles << sq.value(0).toString();
        bible_id_list << sq.value(1).toString();
    }
    sq.clear();

    // Fill bibles comboboxes
    ui->comboBoxPrimaryBible->clear();
    ui->comboBoxPrimaryBible->addItems(bibles);
    ui->comboBoxSecondaryBible->clear();
    ui->comboBoxSecondaryBible->addItem(tr("None"));
    ui->comboBoxSecondaryBible->addItems(bibles);
    ui->comboBoxTrinaryBible->clear();
    ui->comboBoxTrinaryBible->addItem(tr("None"));
    ui->comboBoxTrinaryBible->addItems(bibles);
    ui->comboBoxOperatorBible->clear();
    ui->comboBoxOperatorBible->addItem(tr("Same as primary Bible"));
    ui->comboBoxOperatorBible->addItems(bibles);

    ui->comboBoxPrimaryBible2->clear();
    ui->comboBoxPrimaryBible2->addItems(bibles);
    ui->comboBoxSecondaryBible2->clear();
    ui->comboBoxSecondaryBible2->addItem(tr("None"));
    ui->comboBoxSecondaryBible2->addItems(bibles);
    ui->comboBoxTrinaryBible2->clear();
    ui->comboBoxTrinaryBible2->addItem(tr("None"));
    ui->comboBoxTrinaryBible2->addItems(bibles);

    // Set current primary bible
    if(bversion.primaryBible == "none")
        ui->comboBoxPrimaryBible->setCurrentIndex(0);
    else
        ui->comboBoxPrimaryBible->setCurrentIndex(bible_id_list.indexOf(bversion.primaryBible));

    if(bversion2.primaryBible == "none")
        ui->comboBoxPrimaryBible2->setCurrentIndex(0);
    else
        ui->comboBoxPrimaryBible2->setCurrentIndex(bible_id_list.indexOf(bversion2.primaryBible));

    // Set current secondary bible
    if(bversion.secondaryBible == "none")
        ui->comboBoxSecondaryBible->setCurrentIndex(0);
    else
        ui->comboBoxSecondaryBible->setCurrentIndex(bible_id_list.indexOf(bversion.secondaryBible)+1);

    if(bversion2.secondaryBible == "none")
        ui->comboBoxSecondaryBible2->setCurrentIndex(0);
    else
        ui->comboBoxSecondaryBible2->setCurrentIndex(bible_id_list.indexOf(bversion2.secondaryBible)+1);

    // Set current trinaty bibile
    if(bversion.trinaryBible == "none")
        ui->comboBoxTrinaryBible->setCurrentIndex(0);
    else
        ui->comboBoxTrinaryBible->setCurrentIndex(bible_id_list.indexOf(bversion.trinaryBible)+1);
    updateSecondaryBibleMenu();

    if(bversion2.trinaryBible == "none")
        ui->comboBoxTrinaryBible2->setCurrentIndex(0);
    else
        ui->comboBoxTrinaryBible2->setCurrentIndex(bible_id_list.indexOf(bversion2.trinaryBible)+1);
    updateSecondaryBibleMenu2();

    // Set current operator bibile
    if(bversion.operatorBible == "same")
        ui->comboBoxOperatorBible->setCurrentIndex(0);
    else
        ui->comboBoxOperatorBible->setCurrentIndex(bible_id_list.indexOf(bversion.operatorBible)+1);
    updateOperatorBibleMenu();
}

void BibleSettingWidget::updateSecondaryBibleMenu()
{
    QString pbible = ui->comboBoxPrimaryBible->currentText();
    QString sbible = ui->comboBoxSecondaryBible->currentText();
    secondary_bibles = bibles;
    secondary_bibles.removeOne(pbible);

    secondary_id_list = bible_id_list;
    secondary_id_list.removeAt(ui->comboBoxPrimaryBible->currentIndex());
    ui->comboBoxSecondaryBible->clear();
    ui->comboBoxSecondaryBible->addItem(tr("None"));
    ui->comboBoxSecondaryBible->addItems(secondary_bibles);

    int i = ui->comboBoxSecondaryBible->findText(sbible);
    if( i != -1 )
        // The same secondary bible is still available
        ui->comboBoxSecondaryBible->setCurrentIndex(i);

    updateTrinaryBibleMenu();
}

void BibleSettingWidget::updateSecondaryBibleMenu2()
{
    QString pbible = ui->comboBoxPrimaryBible2->currentText();
    QString sbible = ui->comboBoxSecondaryBible2->currentText();
    secondary_bibles2 = bibles;
    secondary_bibles2.removeOne(pbible);

    secondary_id_list2 = bible_id_list;
    secondary_id_list2.removeAt(ui->comboBoxPrimaryBible2->currentIndex());
    ui->comboBoxSecondaryBible2->clear();
    ui->comboBoxSecondaryBible2->addItem(tr("None"));
    ui->comboBoxSecondaryBible2->addItems(secondary_bibles2);

    int i = ui->comboBoxSecondaryBible2->findText(sbible);
    if( i != -1 ) // The same secondary bible is still available
        ui->comboBoxSecondaryBible2->setCurrentIndex(i);

    updateTrinaryBibleMenu2();
}

void BibleSettingWidget::updateTrinaryBibleMenu()
{
    if (ui->comboBoxSecondaryBible->currentIndex() == 0)
    {
        ui->comboBoxTrinaryBible->setCurrentIndex(0);
        ui->comboBoxTrinaryBible->setEnabled(false);
    }
    else
    {
        ui->comboBoxTrinaryBible->setEnabled(true);
        QString sbible = ui->comboBoxSecondaryBible->currentText();
        QString tbible = ui->comboBoxTrinaryBible->currentText();
        QStringList trinary_bibles = secondary_bibles;
        trinary_bibles.removeOne(sbible);

        trinary_id_list = secondary_id_list;
        trinary_id_list.removeAt(ui->comboBoxSecondaryBible->currentIndex()-1);
        ui->comboBoxTrinaryBible->clear();
        ui->comboBoxTrinaryBible->addItem(tr("None"));
        ui->comboBoxTrinaryBible->addItems(trinary_bibles);

        int i = ui->comboBoxTrinaryBible->findText(tbible);
        if( i != -1 )
            // The same secondary bible is still available
            ui->comboBoxTrinaryBible->setCurrentIndex(i);
    }
}

void BibleSettingWidget::updateTrinaryBibleMenu2()
{
    if (ui->comboBoxSecondaryBible2->currentIndex() == 0)
    {
        ui->comboBoxTrinaryBible2->setCurrentIndex(0);
        ui->comboBoxTrinaryBible2->setEnabled(false);
    }
    else
    {
        ui->comboBoxTrinaryBible2->setEnabled(true);
        QString sbible = ui->comboBoxSecondaryBible2->currentText();
        QString tbible = ui->comboBoxTrinaryBible2->currentText();
        QStringList trinary_bibles = secondary_bibles2;
        trinary_bibles.removeOne(sbible);

        trinary_id_list2 = secondary_id_list2;
        trinary_id_list2.removeAt(ui->comboBoxSecondaryBible2->currentIndex()-1);
        ui->comboBoxTrinaryBible2->clear();
        ui->comboBoxTrinaryBible2->addItem(tr("None"));
        ui->comboBoxTrinaryBible2->addItems(trinary_bibles);

        int i = ui->comboBoxTrinaryBible2->findText(tbible);
        if( i != -1 )
            // The same secondary bible is still available
            ui->comboBoxTrinaryBible2->setCurrentIndex(i);
    }
}

void BibleSettingWidget::updateOperatorBibleMenu()
{
    QString pbible = ui->comboBoxPrimaryBible->currentText();
    QString obible = ui->comboBoxOperatorBible->currentText();
    QStringList operator_bibles = bibles;
    operator_bibles.removeOne(pbible);

    operator_id_list = bible_id_list;
    operator_id_list.removeAt(ui->comboBoxPrimaryBible->currentIndex());
    ui->comboBoxOperatorBible->clear();
    ui->comboBoxOperatorBible->addItem(tr("Same as primary Bible"));
    ui->comboBoxOperatorBible->addItems(operator_bibles);

    int i = ui->comboBoxOperatorBible->findText(obible);
    if( i != -1 )
        // The same operaotr bible is still available
        ui->comboBoxOperatorBible->setCurrentIndex(i);
}

void BibleSettingWidget::setDispScreen2Visible(bool visible)
{
    ui->checkBoxDisp2->setVisible(visible);
    if(!mySettings.useSameForDisp2)
        ui->groupBoxUseDisp2->setVisible(visible);
}

void BibleSettingWidget::on_comboBoxPrimaryBible_activated(const QString &arg1)
{
    updateSecondaryBibleMenu();
    updateOperatorBibleMenu();
    bversion.settingsChanged = true;
}

void BibleSettingWidget::on_comboBoxPrimaryBible2_activated(const QString &arg1)
{
    updateSecondaryBibleMenu2();
    updateTrinaryBibleMenu2();
    bversion2.settingsChanged = true;
}

void BibleSettingWidget::on_comboBoxSecondaryBible_activated(const QString &arg1)
{
    updateTrinaryBibleMenu();
    bversion.settingsChanged = true;
}

void BibleSettingWidget::on_comboBoxSecondaryBible2_activated(const QString &arg1)
{
    updateTrinaryBibleMenu2();
    bversion2.settingsChanged = true;
}

void BibleSettingWidget::on_comboBoxTrinaryBible_activated(const QString &arg1)
{
    bversion2.settingsChanged = true;
}

void BibleSettingWidget::on_comboBoxTrinaryBible2_activated(const QString &arg1)
{
    bversion2.settingsChanged = true;
}

void BibleSettingWidget::on_comboBoxOperatorBible_activated(const QString &arg1)
{
    bversion2.settingsChanged = true;
}

void BibleSettingWidget::on_checkBoxCommonFont_stateChanged(int arg1)
{
    ui->toolButtonTextFont->setEnabled(arg1);
    if(!isLoading)
    {
        mySettings.isNotCommonFont = arg1;
        mySettings.isChangedNotFont = true;
    }
}

void BibleSettingWidget::on_checkBoxCommonFont2_stateChanged(int arg1)
{
    ui->toolButtonTextFont2->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings2.isNotCommonFont = arg1;
    mySettings2.isChangedNotFont = true;
    }
}

void BibleSettingWidget::on_toolButtonTextFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,mySettings.textFont,this);
    if(ok)
    {
        mySettings.textFont = font;
        mySettings.isChangedTextFont = true;
    }

    ui->labelFont->setText(getFontText(mySettings.textFont));
}

void BibleSettingWidget::on_toolButtonTextFont2_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,mySettings.textFont,this);
    if(ok)
    {
        mySettings2.textFont = font;
        mySettings2.isChangedTextFont = true;
    }

    ui->labelFont->setText(getFontText(mySettings2.textFont));
}

void BibleSettingWidget::on_checkBoxCommonColor_stateChanged(int arg1)
{
    ui->toolButtonTextColor->setEnabled(arg1);
    ui->toolButtonShadowColor->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings.isNotCommonColor = arg1;
    mySettings.isChangedNotColor = true;
    }
}

void BibleSettingWidget::on_checkBoxCommonColor2_stateChanged(int arg1)
{
    ui->toolButtonTextColor2->setEnabled(arg1);
    ui->toolButtonShadowColor2->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings2.isNotCommonColor = arg1;
    mySettings2.isChangedNotColor = true;
    }
}

void BibleSettingWidget::on_toolButtonTextColor_clicked()
{
    QColor c(QColorDialog::getColor(mySettings.textColor,this));
    if(c.isValid())
    {
        mySettings.textColor = c;
        mySettings.isChangedTextColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings.textColor);
    ui->graphicViewTextColor->setPalette(p);
}

void BibleSettingWidget::on_toolButtonTextColor2_clicked()
{
    QColor c(QColorDialog::getColor(mySettings2.textColor,this));
    if(c.isValid())
    {
        mySettings2.textColor = c;
        mySettings2.isChangedTextColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings2.textColor);
    ui->graphicViewTextColor2->setPalette(p);
}

void BibleSettingWidget::on_toolButtonShadowColor_clicked()
{
    QColor c(QColorDialog::getColor(mySettings.textShadowColor,this));
    if(c.isValid())
    {
        mySettings.textShadowColor = c;
        mySettings.isChangedTextShadowColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings.textShadowColor);
    ui->graphicViewShadowColor->setPalette(p);
}

void BibleSettingWidget::on_toolButtonShadowColor2_clicked()
{
    QColor c(QColorDialog::getColor(mySettings2.textShadowColor,this));
    if(c.isValid())
    {
        mySettings2.textShadowColor = c;
        mySettings2.isChangedTextShadowColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings2.textShadowColor);
    ui->graphicViewShadowColor2->setPalette(p);
}

void BibleSettingWidget::on_comboBoxVerticalAling_currentIndexChanged(int index)
{
    mySettings.textAlingmentV = index;
    mySettings.isChangedAlingV = true;
}

void BibleSettingWidget::on_comboBoxVerticalAling2_currentIndexChanged(int index)
{
    mySettings2.textAlingmentV = index;
    mySettings2.isChangedAlingV = true;
}

void BibleSettingWidget::on_comboBoxHorizontalAling_currentIndexChanged(int index)
{
    mySettings.textAlingmentH = index;
    mySettings.isChangedAlingH = true;
}

void BibleSettingWidget::on_comboBoxHorizontalAling2_currentIndexChanged(int index)
{
    mySettings2.textAlingmentH = index;
    mySettings2.isChangedAlingH = true;
}

void BibleSettingWidget::on_checkBoxSameFont_stateChanged(int arg1)
{
    ui->toolButtonCaptionFont->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings.isNotSameFont = arg1;
    mySettings.isChangedNotSameFont = true;
    }
}

void BibleSettingWidget::on_checkBoxSameFont2_stateChanged(int arg1)
{
    ui->toolButtonCaptionFont2->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings2.isNotSameFont = arg1;
    mySettings2.isChangedNotSameFont = true;
    }
}

void BibleSettingWidget::on_toolButtonCaptionFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,mySettings.captionFont,this);
    if(ok)
    {
        mySettings.captionFont = font;
        mySettings.isChangedCapFont = true;
    }

    ui->labelFontCaption->setText(getFontText(mySettings.captionFont));
}

void BibleSettingWidget::on_toolButtonCaptionFont2_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,mySettings2.captionFont,this);
    if(ok)
    {
        mySettings2.captionFont = font;
        mySettings2.isChangedCapFont = true;
    }

    ui->labelFontCaption2->setText(getFontText(mySettings2.captionFont));
}

void BibleSettingWidget::on_checkBoxSameColor_stateChanged(int arg1)
{
    ui->toolButtonCaptionColor->setEnabled(arg1);
    ui->toolButtonCaptionShadowColor->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings.isNotSameColor = arg1;
    mySettings.isChangedNotSameColor = true;
    }
}

void BibleSettingWidget::on_checkBoxSameColor2_stateChanged(int arg1)
{
    ui->toolButtonCaptionColor2->setEnabled(arg1);
    ui->toolButtonCaptionShadowColor2->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings.isNotSameColor = arg1;
    mySettings.isChangedNotSameColor = true;
    }
}

void BibleSettingWidget::on_toolButtonCaptionColor_clicked()
{
    QColor c(QColorDialog::getColor(mySettings.captionColor,this));
    if(c.isValid())
    {
        mySettings.captionColor = c;
        mySettings.isChangedCapColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings.captionColor);
    ui->graphicViewCaptionColor->setPalette(p);
}

void BibleSettingWidget::on_toolButtonCaptionColor2_clicked()
{
    QColor c(QColorDialog::getColor(mySettings2.captionColor,this));
    if(c.isValid())
    {
        mySettings2.captionColor = c;
        mySettings2.isChangedCapColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings2.captionColor);
    ui->graphicViewCaptionColor2->setPalette(p);
}

void BibleSettingWidget::on_toolButtonCaptionShadowColor_clicked()
{
    QColor c(QColorDialog::getColor(mySettings.captionShadowColor,this));
    if(c.isValid())
    {
        mySettings.captionShadowColor = c;
        mySettings.isChangedCapShadColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings.captionShadowColor);
    ui->graphicViewCaptionShadowColor->setPalette(p);
}

void BibleSettingWidget::on_toolButtonCaptionShadowColor2_clicked()
{
    QColor c(QColorDialog::getColor(mySettings2.captionShadowColor,this));
    if(c.isValid())
    {
        mySettings2.captionShadowColor = c;
        mySettings2.isChangedCapShadColor = true;
    }
    QPalette p;
    p.setColor(QPalette::Base,mySettings2.captionShadowColor);
    ui->graphicViewCaptionShadowColor2->setPalette(p);
}

void BibleSettingWidget::on_comboBoxCaptionPosition_currentIndexChanged(int index)
{
    mySettings.captionPosition = index;
    mySettings.isChangedCapPos = true;
}

void BibleSettingWidget::on_comboBoxCaptionPosition2_currentIndexChanged(int index)
{
    mySettings2.captionPosition = index;
    mySettings2.isChangedCapPos = true;
}

void BibleSettingWidget::on_comboBoxCaptionAlign_currentIndexChanged(int index)
{
    mySettings.captionAlingment = index;
    mySettings.isChangedCapAlign = true;
}

void BibleSettingWidget::on_comboBoxCaptionAlign2_currentIndexChanged(int index)
{
    mySettings2.captionAlingment = index;
    mySettings2.isChangedCapAlign = true;
}

void BibleSettingWidget::on_comboBoxTransitionType_currentIndexChanged(int index)
{
    mySettings.transitionType = index - 1;
    mySettings.isChangesTranType = true;
}

void BibleSettingWidget::on_comboBoxTransitionType2_currentIndexChanged(int index)
{
    mySettings2.transitionType = index - 1;
    mySettings2.isChangesTranType = true;
}

void BibleSettingWidget::on_comboBoxTextEffects_currentIndexChanged(int index)
{
    mySettings.effectsType = index - 1;
    mySettings.isChangedEffectType = true;
}

void BibleSettingWidget::on_comboBoxTextEffects2_currentIndexChanged(int index)
{
    mySettings2.effectsType = index - 1;
    mySettings2.isChangedEffectType = true;
}

void BibleSettingWidget::on_comboBoxBackgoundType_currentIndexChanged(int index)
{
    mySettings.backgroundType = index - 1;

    ui->toolButtonBrowseBackground->setVisible(mySettings.backgroundType != -1);
    ui->graphicsViewBackgroundColor->setVisible(mySettings.backgroundType != -1);
    ui->lineEditBackPath->setVisible(mySettings.backgroundType != -1);

    if(mySettings.backgroundType == 0)
    {
        ui->graphicsViewBackgroundColor->setVisible(true);
        ui->lineEditBackPath->setVisible(false);
        ui->toolButtonBrowseBackground->setText(tr("Select Color..."));
    }
    else if(mySettings.backgroundType >= 1)
    {
        if(mySettings.backgroundType == 1)
            ui->lineEditBackPath->setText(mySettings.backgroundName);
        else if(mySettings.backgroundType == 2)
            ui->lineEditBackPath->setText(mySettings.backgroundVideoPath);
        ui->graphicsViewBackgroundColor->setVisible(false);
        ui->lineEditBackPath->setVisible(true);
        ui->toolButtonBrowseBackground->setText(tr("Browse..."));
    }
    mySettings.isChangedBackType = true;
}

void BibleSettingWidget::on_comboBoxBackgoundType2_currentIndexChanged(int index)
{
    mySettings2.backgroundType = index - 1;

    ui->toolButtonBrowseBackground2->setVisible(mySettings2.backgroundType != -1);
    ui->graphicsViewBackgroundColor2->setVisible(mySettings2.backgroundType != -1);
    ui->lineEditBackPath2->setVisible(mySettings2.backgroundType != -1);

    if(mySettings2.backgroundType == 0)
    {
        ui->graphicsViewBackgroundColor2->setVisible(true);
        ui->lineEditBackPath2->setVisible(false);
        ui->toolButtonBrowseBackground2->setText(tr("Select Color..."));
    }
    else if(mySettings2.backgroundType >= 1)
    {
        if(mySettings2.backgroundType == 1)
            ui->lineEditBackPath2->setText(mySettings2.backgroundName);
        else if(mySettings2.backgroundType == 2)
            ui->lineEditBackPath2->setText(mySettings2.backgroundVideoPath);
        ui->graphicsViewBackgroundColor2->setVisible(false);
        ui->lineEditBackPath2->setVisible(true);
        ui->toolButtonBrowseBackground2->setText(tr("Browse..."));
    }
 mySettings2.isChangedBackType = true;
}

void BibleSettingWidget::on_toolButtonBrowseBackground_clicked()
{
    QString filename;
    if (mySettings.backgroundType == 0)
    {
        QColor c(QColorDialog::getColor(mySettings.backgroundColor,this));
        if(c.isValid())
        {
            mySettings.backgroundColor = c;
            mySettings.isChangedBackColor = true;
        }
        QPalette p;
        p.setColor(QPalette::Base,mySettings.backgroundColor);
        ui->graphicsViewBackgroundColor->setPalette(p);
    }
    else if (mySettings.backgroundType == 1)
    {
        filename = QFileDialog::getOpenFileName(this, tr("Select a image for Bible wallpaper"),
                                                ".", tr("Images(%1)").arg(getSupportedImageFormats()));
        if(!filename.isNull())
        {
            QPixmap pix(filename);
            if(pix.width()>1280 || pix.height()>1280)
                mySettings.backgroundPix = pix.scaled(1280,1280,Qt::KeepAspectRatio);
            else
                mySettings.backgroundPix = pix;
            QFileInfo fip(filename);
            filename = fip.fileName();
            mySettings.backgroundName = filename;
            ui->lineEditBackPath->setText(filename);
            mySettings.isChangedBackPix = true;
        }
    }
    else if (mySettings.backgroundType == 2)
    {
        filename = QFileDialog::getOpenFileName(this, tr("Select a video for Bible wallpaper"),
                                                ".", "*");
        if(!filename.isNull())
        {
            mySettings.backgroundVideoPath = filename;
            QFileInfo fiv(filename);
            filename = fiv.fileName();

            ui->lineEditBackPath->setText(filename);
            mySettings.isChangedBackVid = true;
        }
    }
}

void BibleSettingWidget::on_toolButtonBrowseBackground2_clicked()
{
    QString filename;
    if (mySettings2.backgroundType == 0)
    {
        QColor c(QColorDialog::getColor(mySettings2.backgroundColor,this));
        if(c.isValid())
        {
            mySettings2.backgroundColor = c;
            mySettings2.isChangedBackColor = true;
        }
        QPalette p;
        p.setColor(QPalette::Base,mySettings2.backgroundColor);
        ui->graphicsViewBackgroundColor2->setPalette(p);
    }
    else if (mySettings2.backgroundType == 1)
    {
        filename = QFileDialog::getOpenFileName(this, tr("Select a image for Bible wallpaper"),
                                                ".", tr("Images(%1)").arg(getSupportedImageFormats()));
        if(!filename.isNull())
        {
            QPixmap pix(filename);
            if(pix.width()>1280 || pix.height()>1280)
                mySettings2.backgroundPix = pix.scaled(1280,1280,Qt::KeepAspectRatio);
            else
                mySettings2.backgroundPix = pix;
            QFileInfo fip(filename);
            filename = fip.fileName();
            mySettings2.backgroundName = filename;
            ui->lineEditBackPath2->setText(filename);
            mySettings2.isChangedBackPix = true;
        }
    }
    else if (mySettings2.backgroundType == 2)
    {
        filename = QFileDialog::getOpenFileName(this, tr("Select a video for Bible wallpaper"),
                                                ".", "*");
        if(!filename.isNull())
        {
            mySettings2.backgroundVideoPath = filename;
            QFileInfo fiv(filename);
            filename = fiv.fileName();

            ui->lineEditBackPath2->setText(filename);
            mySettings2.isChangedBackVid = true;
        }
    }
}

void BibleSettingWidget::on_checkBoxAbbiviations_stateChanged(int arg1)
{
    mySettings.useAbbriviation = arg1;
    mySettings.isChangedUseAbbriv = true;
}

void BibleSettingWidget::on_checkBoxAbbiviations2_stateChanged(int arg1)
{
    mySettings2.useAbbriviation = arg1;
    mySettings2.isChangedUseAbbriv = true;
}

void BibleSettingWidget::on_checkBoxCommonLayout_stateChanged(int arg1)
{
    ui->spinBoxMaxScreen->setEnabled(arg1);
    ui->comboBoxScreenPosition->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings.isNotCommonLayout = arg1;
    mySettings.isChangedNotLayout = true;
    }
}

void BibleSettingWidget::on_checkBoxCommonLayout2_stateChanged(int arg1)
{
    ui->spinBoxMaxScreen2->setEnabled(arg1);
    ui->comboBoxScreenPosition2->setEnabled(arg1);
    if(!isLoading)
    {
    mySettings2.isNotCommonLayout = arg1;
    mySettings2.isChangedNotLayout = true;
    }
}

void BibleSettingWidget::on_spinBoxMaxScreen_editingFinished()
{
    mySettings.screenUse = ui->spinBoxMaxScreen->value();
    mySettings.isChangedScreenUse = true;
}

void BibleSettingWidget::on_spinBoxMaxScreen2_editingFinished()
{
    mySettings2.screenUse = ui->spinBoxMaxScreen2->value();
    mySettings2.isChangedScreenUse = true;
}

void BibleSettingWidget::on_comboBoxScreenPosition_currentIndexChanged(int index)
{
    mySettings.screenPosition = index;
    mySettings.isChangedScreenPos = true;
}

void BibleSettingWidget::on_comboBoxScreenPosition2_currentIndexChanged(int index)
{
    mySettings2.screenPosition = index;
    mySettings2.isChangedScreenPos = true;
}

void BibleSettingWidget::on_checkBoxDisp2_stateChanged(int arg1)
{
    mySettings.useSameForDisp2 = arg1;
    mySettings.isChangedSameDisp2 = true;
    ui->groupBoxUseDisp2->setVisible(!arg1);
}

void BibleSettingWidget::on_pushButtonDefault_clicked()
{
    BibleSettings b;
    mySettings = b;
    mySettings2 = b;
    loadSettings();
}

QString BibleSettingWidget::getFontText(QFont font)
{
    QString f = font.toString().split(",").at(0);
    QString st(QString("%1: %2").arg(f).arg(font.pointSize()));
    if(font.bold())
        st += ", " + tr("Bold");
    if(font.italic())
        st += ", " + tr("Italic");
    if(font.strikeOut())
        st += ", " + tr("StrikeOut");
    if(font.underline())
        st += ", " + tr("Underline");

    return st;
}
