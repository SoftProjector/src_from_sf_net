/***************************************************************************
//
//    softProjector - an open source media projection software
//    Copyleft (Ɔ) 2013  Vladislav Kobzar, Matvey Adzhigirey and Ilya Spivakov
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

#include "settings.h"

GeneralSettings::GeneralSettings()
{
    // Apply General Defauls
    displayIsOnTop = false;
    displayScreen = 0;
    displayScreen2 = -1; // interger "-1" mean "None" or not to display
    currentThemeId = "0";
}

DisplayControlsSettings::DisplayControlsSettings()
{
    buttonSize = 3; // 0=16,1=24,2=32,3=48,4=64,5=96
    alignmentV = 2;
    alignmentH = 1;
    opacity = .3;
}

SpSettings::SpSettings()
{
    // Apply main window defaults
    isWindowMaximized = false;
    uiTranslation = "en";
}

Settings::Settings()
{

}

void Settings::loadSettings()
{
    QString t,n,v,s,sets; // type, name, value, userValues
    QStringList set,values;
    bool dataGenOk = false;
    bool dataSpOk = false;
    QSqlQuery sq;
    sq.exec(QString("SELECT type, sets FROM Settings "));
    while (sq.next())
    {
        t = sq.value(0).toString();
        sets = sq.value(1).toString();

        if(t == "general") // set general setting values
        {
            dataGenOk = true;
            values = sets.split("\n");
            for(int i(0);i<values.count();++i)
            {
                s = values.at(i);
                set = s.split("=");
                n = set.at(0).trimmed();
                v = set.at(1).trimmed();
                if(n == "displayIsOnTop")
                    general.displayIsOnTop = (v=="true");
                else if(n == "currentThemeId")
                    general.currentThemeId = v;
                else if (n == "displayScreen")
                    general.displayScreen = v.toInt();
                // Display Screen 2 settings
                else if (n == "displayScreen2")
                    general.displayScreen2 = v.toInt();
                // End display screen 2 settings
                else if (n == "dcIconSize")
                    general.displayControls.buttonSize = v.toInt();
                else if (n == "dcAlignment")
                {
                    QStringList alinglist = v.split(",");
                    general.displayControls.alignmentV = alinglist.at(0).toInt();
                    general.displayControls.alignmentH = alinglist.at(1).toInt();
                }
                else if (n == "dcOpacity")
                    general.displayControls.opacity = v.toDouble();
            }
        }
        else if(t == "spMain")
        {
            dataSpOk = true;
            values = sets.split("\n");
            for(int i(0);i<values.count();++i)
            {
                s = values.at(i);
                set = s.split("=");
                n = set.at(0).trimmed();
                v = set.at(1).trimmed();

                if(n=="spSplitter")
                    spMain.spSplitter = textToByt(v);
                else if(n=="bibleHiddenSplitter")
                    spMain.bibleHiddenSplitter = textToByt(v);
                else if(n=="bibleShowSplitter")
                    spMain.bibleShowSplitter = textToByt(v);
                else if(n=="songSplitter")
                    spMain.songSplitter = textToByt(v);
                else if(n=="uiTranslation")
                    spMain.uiTranslation = v;
                else if(n=="isWindowMaximized")
                    spMain.isWindowMaximized = (v=="true");
            }
        }
    }

    // if no data exist, then create
    if(!dataGenOk || !dataSpOk)
        saveNewSettings();
}

void Settings::saveSettings()
{
    QSqlQuery sq;
    QString gset,spset;//general,bible,song,annouce,spmain

    // **** Prepare general settings ***************************************
    if(general.displayIsOnTop)
        gset = "displayIsOnTop = true";
    else
        gset = "displayIsOnTop = false";
    gset += "\ncurrentThemeId = " + general.currentThemeId;
    gset += "\ndisplayScreen = " + QString::number(general.displayScreen);
    gset += "\ndisplayScreen2 = " + QString::number(general.displayScreen2);
    gset += "\ndcIconSize = " + QString::number(general.displayControls.buttonSize);
    gset += QString("\ndcAlignment = %1,%2").arg(general.displayControls.alignmentV).arg(general.displayControls.alignmentH);
    gset += "\ndcOpacity = " + QString::number(general.displayControls.opacity);

    // **** prepare softProjector main settings
    spset += "spSplitter = " + spMain.spSplitter.toHex();
    spset += "\nbibleHiddenSplitter = " + spMain.bibleHiddenSplitter.toHex();
    spset += "\nbibleShowSplitter = " + spMain.bibleShowSplitter.toHex();
    spset += "\nsongSplitter = " + spMain.songSplitter.toHex();
    spset += "\nuiTranslation = " + spMain.uiTranslation;
    if(spMain.isWindowMaximized)
        spset += "\nisWindowMaximized = true";
    else
        spset += "\nisWindowMaximized = false";

    sq.exec(QString("UPDATE Settings SET sets = '%1' WHERE type = 'general'").arg(gset));
    sq.exec(QString("UPDATE Settings SET sets = '%1' WHERE type = 'spMain'").arg(spset));
}

void Settings::saveNewSettings()
{
    QSqlQuery sq;
    sq.exec("INSERT INTO Settings (type, sets) VALUES ('general', 'n=v')");
    sq.exec("INSERT INTO Settings (type, sets) VALUES ('spMain', 'n=v')");

    saveSettings();
}

QByteArray Settings::textToByt(QString text)
{
    QByteArray b;
    b.insert(0,text);
    b = b.fromHex(b);
    return b;
}

