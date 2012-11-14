/***************************************************************************
//
//    softProjector - an open source media projection software
//    Copyright (C) 2011  Vladislav Kobzar, Matvey Adzhigirey and Ilya Spivakov
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

#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <QtGui/QWidget>
#include <QtGui>
#include "song.h"
#include "addsongbookdialog.h"
#include "highlight.h"

namespace Ui {
    class EditWidget;
}

class EditWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(EditWidget)
public:
    explicit EditWidget(QWidget *parent = 0);
    virtual ~EditWidget();

public slots:
    void setCopy(Song copy);
    void setEdit(Song sEdit);
    void setNew();
    QStringList categories();
    void retranslateUis();

signals:
    // For communicating with SongsModel
    void updateSongFromDatabase(int songid, int initial_song_id);
    void addedNew(Song song, int initial_song_id);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::EditWidget *ui;
    Song editSong, newSong;
    SongDatabase song_database;
    bool is_new;
    Highlight *highlight;
    QIntValidator *song_num_validator;
    void setWaitCursor();
    void setArrowCursor();
    QString add_to_songbook;
    QList<int> cat_ids;
    int song_to_edit_id;


private slots:
    void addNewSong(Song song, QString msgNewSongbook, QString msgCaption);
    void addSongbook();
    void on_btnCancel_clicked();
    void on_btnSave_clicked();
    void resetUiItems();
    void setUiItems();
    void setSave();
    void setSongbook(int id);
    void loadCategories(bool ui_update);
    QString resetLyric(QString lyric);
    QString setSongText(QString text);
    int isInDatabase(Song *song);
    void on_checkBoxSongSettings_toggled(bool checked);
    void updateColor();
    void on_pushButtonTextColor_clicked();
    void on_pushButtonTextFont_clicked();
    void on_pushButtonBackgroundPath_clicked();
    void on_pushButtonRemoveBackground_clicked();
};

#endif // EDITWIDGET_H
