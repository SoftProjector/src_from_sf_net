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

#ifndef ANNOUNCEWIDGET_H
#define ANNOUNCEWIDGET_H

#include <QtGui>

namespace Ui {
    class AnnounceWidget;
}


class Announcement
// Class for holding all information about an announcement
{
public:
    QString text;
    int align_flags;
    // FIXME add aligment info, font size, etc.
};


class AnnounceWidget : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(AnnounceWidget)
public:
    explicit AnnounceWidget(QWidget *parent = 0);
    virtual ~AnnounceWidget();
    QString getText();
//    void drawToPainter(QPainter *painter, int width, int height);

protected:
    virtual void changeEvent(QEvent *e);

private slots:
    void on_history_listWidget_currentRowChanged(int currentRow);
    void on_history_listWidget_doubleClicked(QModelIndex index);
    void on_remove_from_history_pushButton_clicked();
    void on_add_to_history_pushButton_clicked();
    void on_btnLive_clicked();
    void sendToProjector();
    int get_align_flags();
    void set_align_boxes(int flags);

signals:
    // To be used ONLY by SongWidget::sendToProjector():
    void sendText(Announcement text);
    void annouceListChanged(bool changed);

private:
    Ui::AnnounceWidget *ui;
    QList<Announcement> history_items;

public slots:
    QList<Announcement> getAnnouncements();
    void loadFromFile(QList<Announcement> anns);
    void setAlingment(int v, int h);
};

#endif // ANNOUNCEWIDGET_H
