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

#ifndef MANAGEDATADIALOG_H
#define MANAGEDATADIALOG_H

#include <QtGui/QDialog>
#include <QtGui>
#include <QtSql>

#include "managedata.h"
#include "song.h"
#include "addsongbookdialog.h"
#include "bibleinformationdialog.h"

namespace Ui {
    class ManageDataDialog;
}

class ManageDataDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ManageDataDialog)
public:
    explicit ManageDataDialog(QWidget *parent = 0);
    virtual ~ManageDataDialog();
    bool reload_bible;
    bool reload_songbook;
    bool reloadThemes;

public slots:
    void load_songbooks();
    void loadThemes();

protected:
    virtual void changeEvent(QEvent *e);

signals:
    void setMainWaitCursor();
    void setMainArrowCursor();

private:
    QList<Bibles> bible_list;
    QList<Songbook> songbook_list;
    QList<ThemeInfo> themeList;
    BiblesModel *bible_model;
    SongbooksModel *songbook_model;
    ThemeModel *themeModel;
    Ui::ManageDataDialog *ui;


private slots:
    QString get3(int i);
    QString getVerseId(QString book, QString chapter, QString verse);
    void setWaitCursor();
    void setArrowCursor();
    void on_songbookTableView_clicked(QModelIndex index);
    void on_bibleTableView_clicked(QModelIndex index);
    void updateBibleButtons();
    void updateSongbookButtons();
    void updateThemeButtons();
    void on_edit_bible_pushButton_clicked();
    void on_edit_songbook_pushButton_clicked();
    void on_delete_bible_pushButton_clicked();
    void on_export_bible_pushButton_clicked();
    void on_import_bible_pushButton_clicked();
    void on_ok_pushButton_clicked();
    void on_delete_songbook_pushButton_clicked();
    void on_export_songbook_pushButton_clicked();
    void on_import_songbook_pushButton_clicked();
    void deleteBible(Bibles bilbe);
    void importBible(QString path, QString version);
    void exportBible(QString path, Bibles bible);
    void deleteSongbook(Songbook songbook);
    void importSongbook(QString path);
    void exportSongbook(QString path);
    void load_bibles();
    void toMultiLine(QString& mline);
    void toSingleLine(QString& sline);
    void on_pushButtonThemeNew_clicked();
    void on_pushButtonThemeImport_clicked();
    void on_pushButtonThemeEdit_clicked();
    void on_pushButtonThemeExport_clicked();
    void on_pushButtonThemeDelete_clicked();
    void on_TableViewTheme_clicked(const QModelIndex &index);
    void deleteTheme(ThemeInfo tme);
    void on_pushButtonThemeExportAll_clicked();
    void exportTheme(QXmlStreamWriter &xml, ThemeInfo &tmInfo);
    void importTheme(QXmlStreamReader &xml);
};


#endif // MANAGEDATADIALOG_H
