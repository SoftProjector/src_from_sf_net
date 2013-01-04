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

#ifndef BIBLEINFORMATIONDIALOG_H
#define BIBLEINFORMATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class BibleInformationDialog;
}

class BibleInformationDialog : public QDialog
{
    Q_OBJECT

public:
    QString title;
    QString abbr;
    QString info;
    bool isRtoL;
    explicit BibleInformationDialog(QWidget *parent = 0);
    ~BibleInformationDialog();

public slots:
    void setBibleIformation(QString title, QString abbreviation, QString info, bool is_rtol);

private:
    Ui::BibleInformationDialog *ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // BIBLEINFORMATIONDIALOG_H
