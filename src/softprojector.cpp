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

#include <QDesktopWidget>
#include "softprojector.h"
#include "ui_softprojector.h"
#include "aboutdialog.h"
#include "editannouncementdialog.h"

SoftProjector::SoftProjector(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SoftProjectorClass)
{
    // Load settings
    mySettings.loadSettings();
    theme.setThemeId(mySettings.general.currentThemeId);
    theme.loadTheme();
    // Reset current theme id if initial was 0
    mySettings.general.currentThemeId = theme.getThemeId();

    //Setting up the Display Screen
    desktop = new QDesktopWidget();
    // NOTE: With virtual desktop, desktop->screen() will always return the main screen,
    // so this will initialize the Display1 widget on the main screen:
    displayScreen1 = new DisplayScreen(desktop->screen(0));
    displayScreen2 = new DisplayScreen(desktop->screen(0)); //for future
    // Don't worry, we'll move it later

    bibleWidget = new BibleWidget;
    songWidget = new SongWidget;
    editWidget = new EditWidget;
    announceWidget = new AnnounceWidget;
    manageDialog = new ManageDataDialog(this);
    settingsDialog = new SettingsDialog(this);
    helpDialog = new HelpDialog();
    pictureWidget = new PictureWidget;
    mediaPlayer = new MediaWidget;

    ui->setupUi(this);


    // Create action group for language slections
    languagePath = "."+QString(QDir::separator())+"translations"+QString(QDir::separator());
    createLanguageActions();

    // Always place the "Settings" menu item under the application
    // menu, even if the item is translated (Mac OS X):
    ui->actionSettings->setMenuRole(QAction::PreferencesRole);
    // FIXME Make the Preferences menu appear in the menu bar even for the
    // display window (Mac OS X)

    // Apply Settings
    applySetting(mySettings.general, theme, mySettings.bibleSets, mySettings.bibleSets2);

    displayScreen1->setWindowIcon(QIcon(":icons/icons/display.png"));
    displayScreen2->setWindowIcon(QIcon(":icons/icons/display.png"));

    positionDisplayWindow();

    displayScreen1->renderText(false);
    displayScreen2->renderText(false);

    showing = false;

    ui->projectTab->clear();
    ui->projectTab->addTab(bibleWidget,QIcon(":/icons/icons/book.png"), tr("Bible (F6)"));
    ui->projectTab->addTab(songWidget,QIcon(":/icons/icons/song_tab.png"), tr("Songs (F7)"));
    ui->projectTab->addTab(pictureWidget,QIcon(":/icons/icons/photo.png"),tr("Pictures"));
    ui->projectTab->addTab(mediaPlayer,QIcon(":/icons/icons/video.png"),tr("Media"));
    ui->projectTab->addTab(announceWidget,QIcon(":/icons/icons/announce.png"), tr("Announcements (F8)"));
    ui->projectTab->setCurrentIndex(0);


    connect(bibleWidget, SIGNAL(goLive(QStringList, QString, QItemSelection)),
            this, SLOT(setChapterList(QStringList, QString, QItemSelection)));
    connect(bibleWidget, SIGNAL(setArrowCursor()), this, SLOT(setArrowCursor()));
    connect(bibleWidget, SIGNAL(setWaitCursor()), this, SLOT(setWaitCursor()));
    connect(songWidget, SIGNAL(sendSong(Song, int)), this, SLOT(setSongList(Song, int)));
    connect(songWidget, SIGNAL(setArrowCursor()), this, SLOT(setArrowCursor()));
    connect(songWidget, SIGNAL(setWaitCursor()), this, SLOT(setWaitCursor()));
    connect(announceWidget,SIGNAL(sendAnnounce(Announcement,int)), this, SLOT(setAnnounceText(Announcement,int)));
    connect(pictureWidget, SIGNAL(sendSlideShow(QList<SlideShowItem>&,int)), this, SLOT(setPictureList(QList<SlideShowItem>&,int)));
    connect(pictureWidget, SIGNAL(sendToSchedule(SlideShow&)),this,SLOT(addToShcedule(SlideShow&)));
    connect(mediaPlayer, SIGNAL(toProjector(VideoInfo&)), this, SLOT(setVideo(VideoInfo&)));
    connect(editWidget, SIGNAL(updateSongFromDatabase(int,int)), songWidget, SLOT(updateSongFromDatabase(int,int)));
    connect(editWidget, SIGNAL(addedNew(Song,int)), songWidget,SLOT(addNewSong(Song,int)));
    connect(manageDialog, SIGNAL(setMainArrowCursor()), this, SLOT(setArrowCursor()));
    connect(manageDialog, SIGNAL(setMainWaitCursor()), this, SLOT(setWaitCursor()));
    connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(switchLanguage(QAction*)));
    connect(displayScreen1,SIGNAL(exitSlide()),this,SLOT(on_clear_button_clicked()));
    connect(displayScreen1,SIGNAL(nextSlide()),this,SLOT(nextSlide()));
    connect(displayScreen1,SIGNAL(prevSlide()),this,SLOT(prevSlide()));
    connect(settingsDialog,SIGNAL(updateSettings(GeneralSettings&,Theme&,BibleVersionSettings&,BibleVersionSettings&)),
            this,SLOT(updateSetting(GeneralSettings&,Theme&,BibleVersionSettings&,BibleVersionSettings&)));
    connect(settingsDialog,SIGNAL(positionsDisplayWindow()),this,SLOT(positionDisplayWindow()));
    connect(settingsDialog,SIGNAL(updateScreen()),this,SLOT(updateScreen()));
    connect(songWidget,SIGNAL(addToSchedule(Song&)),this,SLOT(addToShcedule(Song&)));
    connect(announceWidget,SIGNAL(addToSchedule(Announcement&)),this,SLOT(addToShcedule(Announcement&)));

    ui->toolBar->addAction(ui->actionNewSchedule);
    ui->toolBar->addAction(ui->actionOpenSchedule);
    ui->toolBar->addAction(ui->actionSaveSchedule);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionPrint);
    ui->toolBar->addSeparator();
    ui->toolBar_2->addAction(ui->actionMoveScheduleTop);
    ui->toolBar_2->addAction(ui->actionMoveScheduleUp);
    ui->toolBar_2->addAction(ui->actionMoveScheduleDown);
    ui->toolBar_2->addAction(ui->actionMoveScheduleBottom);
    ui->toolBar_2->addSeparator();
    ui->toolBar_2->addAction(ui->actionScheduleAdd);
    ui->toolBar_2->addAction(ui->actionScheduleRemove);
    ui->toolBar_2->addAction(ui->actionScheduleClear);
//    ui->toolBar_2->setIconSize(QSize(24,24));
    //ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionEdit);
    ui->toolBar->addAction(ui->actionCopy);
    ui->toolBar->addAction(ui->actionDelete);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionSettings);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionSong_Counter);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->action_Help);

    ui->show_button->setEnabled(false);
    ui->clear_button->setEnabled(false);

    // Create and connect shortcuts
    shpgUP = new QShortcut(Qt::Key_PageUp,this);
    shpgDwn = new QShortcut(Qt::Key_PageDown,this);
    shSart1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B),this);
    shSart2 = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F5),this);
    connect(shpgUP,SIGNAL(activated()),this,SLOT(prevSlide()));
    connect(shpgDwn,SIGNAL(activated()),this,SLOT(nextSlide()));
    connect(shSart1,SIGNAL(activated()),this,SLOT(on_show_button_clicked()));
    connect(shSart2,SIGNAL(activated()),this,SLOT(on_show_button_clicked()));

    // Hide Multi verse selection, only visible to be when showing bible
    ui->widgetMultiVerse->setVisible(false);

    // Set up video controls
    playerSlider = new Phonon::SeekSlider(this);
    playerSlider->setMediaObject(displayScreen1->videoPlayer);
    ui->horizontalLayoutPlayBackTime->insertWidget(1,playerSlider);

    playerAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory);
    volumeSlider = new Phonon::VolumeSlider(playerAudioOutput);
    Phonon::createPath(displayScreen1->videoPlayer,playerAudioOutput);
    ui->horizontalLayoutPlayBackButtons->addWidget(volumeSlider);

    connect(displayScreen1, SIGNAL(sendTimeText(QString)),this, SLOT(setTimeText(QString)));
    connect(displayScreen1, SIGNAL(updatePlayButton(bool)),this,SLOT(setButtonPlayIcon(bool)));

    ui->widgetPlayBackControls->setVisible(false);

//    version_string = "2"; // to be used only for official release
    version_string = "2.Dev.Build: 6"; // to be used between official releases
    this->setWindowTitle("softProjector " + version_string);

}

SoftProjector::~SoftProjector()
{
    saveSettings();
    delete songWidget;
    delete editWidget;
    delete bibleWidget;
    delete announceWidget;
    delete manageDialog;
    delete playerSlider;
    delete playerAudioOutput;
    delete volumeSlider;
    delete mediaPlayer;
    delete displayScreen1;
    delete displayScreen2;
    delete desktop;
    delete languageGroup;
    delete settingsDialog;
    delete shpgUP;
    delete shpgDwn;
    delete shSart1;
    delete shSart2;
    delete helpDialog;
    delete ui;
}

void SoftProjector::positionDisplayWindow()
{
    // Position the display window as needed (including setting "always on top" flag,
    // showing full screen / normal mode, and positioning it on the right screen)

    if (mySettings.general.displayIsOnTop)
    {
        displayScreen1->setWindowFlags(Qt::WindowStaysOnTopHint);
        displayScreen2->setWindowFlags(Qt::WindowStaysOnTopHint);
    }
    else
    {
        displayScreen1->setWindowFlags(0); // Do not show always on top
        displayScreen2->setWindowFlags(0); // Do not show always on top
    }

    if(desktop->screenCount() > 1)
    {
        if (desktop->isVirtualDesktop())
        {
            // Move the display widget to screen 1 (secondary screen):
            QPoint top_left = desktop->screenGeometry(mySettings.general.displayScreen).topLeft();
            displayScreen1->move(top_left);
        }
        displayScreen1->showFullScreen();
        displayScreen1->setCursor(Qt::BlankCursor); //Sets a Blank Mouse to the screen
        displayScreen1->positionOpjects();
        displayScreen1->setControlButtonsVisible(false);

        // check if to display secondary display screen
        if(mySettings.general.displayScreen2>=0)
        {
            hasDisplayScreen2 = true;
            if (desktop->isVirtualDesktop())
            {
                // Move the display widget to screen 1 (secondary screen):
                QPoint top_left = desktop->screenGeometry(mySettings.general.displayScreen2).topLeft();
                displayScreen2->move(top_left);
            }
            displayScreen2->showFullScreen();
            displayScreen2->setCursor(Qt::BlankCursor); //Sets a Blank Mouse to the screen
            displayScreen2->positionOpjects();
            displayScreen2->setControlButtonsVisible(false);
        }
        else
        {
            hasDisplayScreen2 = false;
            displayScreen2->hide();
        }

        // specify that there is more than one diplay screen(monitor) availbale
        isSingleScreen = false;
    }
    else
    {
        // Single monitor only: Do not show on strat up.
        // Will be shown only when items were sent to the projector.
        showDisplayScreen(false);
        isSingleScreen = true;
        hasDisplayScreen2 = false;
    }
}

void SoftProjector::showDisplayScreen(bool show)
{
    if(show)
    {
        displayScreen1->showFullScreen();
        displayScreen1->positionOpjects();
    }
    else
        displayScreen1->hide();
    displayScreen1->setControlsSettings(mySettings.general.displayControls);
    displayScreen1->setControlButtonsVisible(true);
}

void SoftProjector::saveSettings()
{
    // Save splitter states
    mySettings.spMain.spSplitter = ui->splitter->saveState();
    mySettings.spMain.bibleHiddenSplitter = bibleWidget->getHiddenSplitterState();
    mySettings.spMain.bibleShowSplitter = bibleWidget->getShownSplitterState();
    mySettings.spMain.songSplitter = songWidget->getSplitterState();

    // Save window maximized state
    mySettings.spMain.isWindowMaximized = this->isMaximized();

    // save translation settings
    QList<QAction*> languageActions = ui->menuLanguage->actions();

    for(int i(0);i < languageActions.count();++i)
    {
        if(languageActions.at(i)->isChecked())
        {
            if(i < languageActions.count())
                mySettings.spMain.uiTranslation = languageActions.at(i)->data().toString();
            else
                mySettings.spMain.uiTranslation = "en";
        }
    }

    // save settings
//    mySettings.general.currentThemeId = theme.getThemeId();
    mySettings.saveSettings();
    theme.saveThemeUpdate();
}

void SoftProjector::updateSetting(GeneralSettings &g, Theme &t, BibleVersionSettings &bsets, BibleVersionSettings &bsets2)
{
    mySettings.general = g;
    mySettings.bibleSets = bsets;
    mySettings.bibleSets2 = bsets2;
    mySettings.saveSettings();
    theme = t;
    bibleWidget->setSettings(mySettings.bibleSets);
    
    // Apply display settings;
    displayScreen1->setNewPassiveWallpaper(theme.passive.background,theme.passive.useBackground);
    if(theme.passive2.useDisp2settings)
        displayScreen2->setNewPassiveWallpaper(theme.passive2.background,theme.passive2.useBackground);
    else
        displayScreen2->setNewPassiveWallpaper(theme.passive.background,theme.passive.useBackground);
}

void SoftProjector::applySetting(GeneralSettings &g, Theme &t, BibleVersionSettings &b1, BibleVersionSettings &b2)
{
    updateSetting(g,t,b1,b2);

    // Apply splitter states
    ui->splitter->restoreState(mySettings.spMain.spSplitter);
    bibleWidget->setHiddenSplitterState(mySettings.spMain.bibleHiddenSplitter);
    bibleWidget->setShownSplitterState(mySettings.spMain.bibleShowSplitter);
    songWidget->setSplitterState(mySettings.spMain.songSplitter);

    // Apply window maximized
    if(mySettings.spMain.isWindowMaximized)
        this->setWindowState(Qt::WindowMaximized);

    // Apply current translation
    QList<QAction*> la = ui->menuLanguage->actions();
    QString splocale;
    for(int i(0);i < la.count(); ++i)
    {
        if(la.at(i)->data().toString() == mySettings.spMain.uiTranslation)
        {
            if(i < la.count())
            {
                ui->menuLanguage->actions().at(i)->setChecked(true);
                splocale = mySettings.spMain.uiTranslation;
            }
            else
            {
                ui->menuLanguage->actions().at(0)->setChecked(true);//default
                splocale = "en";
            }
        }
    }
    cur_locale = splocale;
    retranslateUis();
}

void SoftProjector::closeEvent(QCloseEvent *event)
{
    if(is_schedule_saved || schedule_file_path.isEmpty())
    {
        QCoreApplication::exit(0);
        event->accept();
    }
    else
    {
        QMessageBox mb;
        mb.setWindowTitle(tr("Schedule not saved"));
        mb.setText(tr("Do you want to save current schedule?"));
        mb.setIcon(QMessageBox::Question);
        mb.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);
        mb.setDefaultButton(QMessageBox::Save);
        int ret = mb.exec();

        switch (ret)
        {
        case QMessageBox::Save:
            // Save Schedule and close
            on_actionSaveSchedule_triggered();
            QCoreApplication::exit(0);
            event->accept();
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked, do nothing
            event->ignore();
            break;
        case QMessageBox::Discard:
            // Close without saving
            QCoreApplication::exit(0);
            event->accept();
            break;
        default:
            // should never be reached
            break;
        }
    }
}

void SoftProjector::keyPressEvent(QKeyEvent *event)
{
    // Will get called when a key is pressed
    int key = event->key();
    if(key == Qt::Key_F6)
        ui->projectTab->setCurrentWidget(bibleWidget);
    else if(key == Qt::Key_F7)
        ui->projectTab->setCurrentWidget(songWidget);
    else if(key == Qt::Key_F8)
        ui->projectTab->setCurrentWidget(announceWidget);
    else if(key == Qt::Key_Left)
        prevSlide();
    else if(key == Qt::Key_Back)
        prevSlide();
    else if(key == Qt::Key_Right)
        nextSlide();
    else if(key == Qt::Key_Forward)
        nextSlide();
    else if(key == Qt::Key_Return)
        nextSlide();
    else if(key == Qt::Key_Enter)
        nextSlide();
    else
        QMainWindow::keyPressEvent(event);
}

void SoftProjector::on_actionClose_triggered()
{
    close();
}

void SoftProjector::setAnnounceText(Announcement announce, int row)
{
    currentAnnounce = announce;
    type = "announce";
    ui->widgetMultiVerse->setVisible(false);
    ui->rbMultiVerse->setChecked(false);
    ui->widgetPlayBackControls->setVisible(false);
    showing = true;
    new_list = true;
    ui->labelIcon->setPixmap(QPixmap(":/icons/icons/announce.png").scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->labelShow->setText(currentAnnounce.title);
    ui->listShow->clear();
    ui->listShow->setSpacing(5); // ?
    ui->listShow->setWordWrap(true);
    ui->listShow->addItems(currentAnnounce.getAnnounceList());
    ui->listShow->setCurrentRow(row);
    ui->listShow->setFocus();
    new_list = false;
    updateScreen();
}

void SoftProjector::setSongList(Song song, int row)
{
    QStringList song_list = song.getSongTextList();
    current_song = song;
    current_song_verse = row;

    // Display the specified song text in the right-most column of softProjector
    type = "song";
    ui->widgetMultiVerse->setVisible(false);
    ui->rbMultiVerse->setChecked(false);
    ui->widgetPlayBackControls->setVisible(false);
    showing = true;
    new_list = true;
    ui->listShow->clear();
    ui->labelIcon->setPixmap(QPixmap(":/icons/icons/song_tab.png").scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->labelShow->setText(song.title);
    ui->listShow->setSpacing(5);
    ui->listShow->setWordWrap(false);

    ui->listShow->addItems(song_list);
    ui->listShow->setCurrentRow(row);
    ui->listShow->setFocus();
    new_list = false;
    updateScreen();
}

void SoftProjector::setChapterList(QStringList chapter_list, QString caption, QItemSelection selectedItems)
{
    // Called to show a bible verse from a chapter in the preview list

    type = "bible";
    ui->widgetMultiVerse->setVisible(true);
    ui->widgetPlayBackControls->setVisible(false);
    showing = true;
    new_list = true;
    ui->labelIcon->setPixmap(QPixmap(":/icons/icons/book.png").scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->labelShow->setText(caption);
    ui->listShow->clear();
    ui->listShow->setSpacing(2);
    ui->listShow->setWordWrap(true);
    ui->listShow->addItems(chapter_list);
    if(selectedItems.indexes().count()>1)
        ui->rbMultiVerse->setChecked(true);
    else
        ui->rbMultiVerse->setChecked(false);

    ui->listShow->setCurrentRow(selectedItems.first().top());
    ui->listShow->selectionModel()->select(selectedItems,QItemSelectionModel::Select);
    ui->listShow->setFocus();
    new_list = false;
    updateScreen();
}

void SoftProjector::setPictureList(QList<SlideShowItem> &image_list,int row)
{
    // Called to show picture list
    type = "pix";
    showing = true;
    ui->widgetMultiVerse->setVisible(false);
    ui->rbMultiVerse->setChecked(false);
    ui->widgetPlayBackControls->setVisible(false);
    new_list = true;
    pictureShowList = image_list;
    ui->labelIcon->setPixmap(QPixmap(":/icons/icons/photo.png").scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->labelShow->clear();
    ui->listShow->clear();
    ui->listShow->setSpacing(1);
    ui->listShow->setIconSize(QSize(100,100));

    foreach(const SlideShowItem &p, pictureShowList)
    {
        QListWidgetItem *itm = new QListWidgetItem;
        QIcon ico(p.imageSmall);
        itm->setIcon(ico);
        ui->listShow->addItem(itm);
    }

    ui->listShow->setCurrentRow(row);
    ui->listShow->setFocus();
    new_list = false;
    updateScreen();
}

void SoftProjector::setVideo(VideoInfo &video)
{
    // Called when showing video
    type = "video";
    currentVideo = video;
    showing = true;
    ui->widgetMultiVerse->setVisible(false);
    ui->rbMultiVerse->setChecked(false);
    if(!ui->widgetPlayBackControls->isVisible())
        ui->widgetPlayBackControls->setVisible(true);
    new_list = true;
    ui->listShow->clear();
    ui->labelIcon->setPixmap(QPixmap(":/icons/icons/video.png").scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
//    ui->listShow->addItem(videoPath);
//    ui->listShow->setCurrentRow(0);
    ui->labelShow->setText(currentVideo.fileName);
    new_list = false;
    updateScreen();
}

void SoftProjector::on_listShow_currentRowChanged(int currentRow)
{
    // Called when the user selects a different row in the show (right-most) list.
    //    updateScreen();
}

void SoftProjector::on_listShow_itemSelectionChanged()
{
    // Called when the user selects a different row in the show (right-most) list.
    // First check if ratio button "Multi Verse" is check. If so, make button "Show"
    // enable and update screen only after show_botton is clicked.
    if(ui->rbMultiVerse->isChecked())
        ui->show_button->setEnabled(true);
    else
        updateScreen();
}

void SoftProjector::updateScreen()
{
    // Display the specified row of the show (rightmost) table to
    // the display
    int currentRow = ui->listShow->currentRow();

    if(!showing)
    {
        // Do not display any text:
        displayScreen1->renderText(false);
//        if(type == "video")
//            displayScreen1->videoPlayer->pause();

        if(isSingleScreen)
            showDisplayScreen(false);

        if(hasDisplayScreen2)
            displayScreen2->renderText(false);
        ui->show_button->setEnabled(true);
        ui->clear_button->setEnabled(false);
    }
    else if ((currentRow >=0 && !new_list) || (type == "video" && !new_list))
    {
        if(isSingleScreen)
        {
            if(displayScreen1->isHidden())
                showDisplayScreen(true);
        }

        ui->show_button->setEnabled(false);
        ui->clear_button->setEnabled(true);

        if(type=="bible")
        {
            int srows(ui->listShow->count());
            QList<int> currentRows;
            for(int i(0); i<srows; ++i)
            {
                if(ui->listShow->item(i)->isSelected())
                    currentRows.append(i);
            }
            displayScreen1->renderBibleText(bibleWidget->bible.getCurrentVerseAndCaption(currentRows,theme.bible,mySettings.bibleSets)
                                            ,theme.bible);
            if(hasDisplayScreen2)
            {
                if(theme.bible2.useDisp2settings)
                    displayScreen2->renderBibleText(bibleWidget->bible.getCurrentVerseAndCaption(currentRows,theme.bible2,
                                                                                                 mySettings.bibleSets2),theme.bible2);
                else
                    displayScreen2->renderBibleText(bibleWidget->bible.getCurrentVerseAndCaption(currentRows,theme.bible,
                                                                                                 mySettings.bibleSets),theme.bible);
            }
        }
        else if(type=="song")
        {
            displayScreen1->renderSongText(current_song.getStanza(currentRow),theme.song);
            if(hasDisplayScreen2)
            {
                if(theme.song2.useDisp2settings)
                    displayScreen2->renderSongText(current_song.getStanza(currentRow),theme.song2);
                else
                    displayScreen2->renderSongText(current_song.getStanza(currentRow),theme.song);
            }
        }
        else if(type == "announce")
        {
            displayScreen1->renderAnnounceText(currentAnnounce.getAnnounceSlide(currentRow),theme.announce);
            if(hasDisplayScreen2)
            {
                if(theme.announce2.useDisp2settings)
                    displayScreen2->renderAnnounceText(currentAnnounce.getAnnounceSlide(currentRow),theme.announce2);
                else
                    displayScreen2->renderAnnounceText(currentAnnounce.getAnnounceSlide(currentRow),theme.announce);
            }
        }
        else if(type == "pix")
        {
            displayScreen1->renderPicture(pictureShowList.at(currentRow).image);
        }
        else if(type == "video")
        {
            displayScreen1->renderVideo(currentVideo);
            if(hasDisplayScreen2)
                displayScreen2->renderVideo(currentVideo);
        }
    }

}

void SoftProjector::on_clear_button_clicked()
{
    showing = false;
    updateScreen();
}

void SoftProjector::on_show_button_clicked()
{
    showing = true;
    updateScreen();
}

void SoftProjector::on_actionSettings_triggered()
{
    settingsDialog->loadSettings(mySettings.general,theme,mySettings.bibleSets,mySettings.bibleSets2);
    settingsDialog->exec();
}

void SoftProjector::on_listShow_doubleClicked(QModelIndex index)
{
    // Called when the user double clicks on a row in the preview table.
    showing = true;
    updateScreen();
}

void SoftProjector::on_projectTab_currentChanged(int index)
{
    updateEditActions();
}

void SoftProjector::updateEditActions()
{
    int ctab = ui->projectTab->currentIndex();
    // ctab - 0=bible, 1=songs, 2=pix, 3=media, 4=annouce
    if(ctab == 0)
    {
        ui->actionNew->setText("");
        ui->actionEdit->setText("");
        ui->actionCopy->setText("");
        ui->actionDelete->setText("&Clear Bible History List");
        ui->actionNew->setIcon(QIcon());
        ui->actionEdit->setIcon(QIcon());
        ui->actionCopy->setIcon(QIcon());
        ui->actionDelete->setIcon(QIcon(":/icons/icons/bibleHistoryDelete.png"));
    }
    else if(ctab == 1) // Song Tab
    {
        ui->actionNew->setText(tr("&New Song..."));
        ui->actionEdit->setText(tr("&Edit Song..."));
        ui->actionCopy->setText(tr("&Copy Song..."));
        ui->actionDelete->setText(tr("&Delete Song"));
        ui->actionNew->setIcon(QIcon(":/icons/icons/song_new.png"));
        ui->actionEdit->setIcon(QIcon(":/icons/icons/song_edit.png"));
        ui->actionCopy->setIcon(QIcon(":/icons/icons/song_copy.png"));
        ui->actionDelete->setIcon(QIcon(":/icons/icons/song_delete.png"));
    }
    else if (ctab == 2) // Picture Tab
    {
        ui->actionNew->setText(tr("&New Slide Show..."));
        ui->actionEdit->setText(tr("&Edit Slide Show..."));
        ui->actionCopy->setText("");
        ui->actionDelete->setText(tr("&Delete Slide Show"));
        ui->actionNew->setIcon(QIcon(":/icons/icons/slideshow_new.png"));
        ui->actionEdit->setIcon(QIcon(":/icons/icons/slideshow_edit.png"));
        ui->actionCopy->setIcon(QIcon());
        ui->actionDelete->setIcon(QIcon(":/icons/icons/slideshow_delete.png"));
    }
    else if (ctab == 3) // Media Tab
    {
        ui->actionNew->setText(tr("&Add Media Files..."));
        ui->actionEdit->setText("");
        ui->actionCopy->setText("");
        ui->actionDelete->setText(tr("&Remove Media Files"));
        ui->actionNew->setIcon(QIcon(":/icons/icons/video_add.png"));
        ui->actionEdit->setIcon(QIcon());
        ui->actionCopy->setIcon(QIcon());
        ui->actionDelete->setIcon(QIcon(":/icons/icons/video_remove.png"));
    }
    else if (ctab == 4) // Announcement Tab
    {
        ui->actionNew->setText(tr("&New Announcement..."));
        ui->actionEdit->setText(tr("&Edit Announcement..."));
        ui->actionCopy->setText(tr("&Copy Announcement..."));
        ui->actionDelete->setText(tr("&Delete Announcement"));
        ui->actionNew->setIcon(QIcon(":/icons/icons/announce_new.png"));
        ui->actionEdit->setIcon(QIcon(":/icons/icons/announce_edit.png"));
        ui->actionCopy->setIcon(QIcon(":/icons/icons/announce_copy.png"));
        ui->actionDelete->setIcon(QIcon(":/icons/icons/announce_delete.png"));
    }
    else
    {
        ui->actionNew->setText("");
        ui->actionEdit->setText("");
        ui->actionCopy->setText("");
        ui->actionDelete->setText("");
        ui->actionNew->setIcon(QIcon());
        ui->actionEdit->setIcon(QIcon());
        ui->actionCopy->setIcon(QIcon());
        ui->actionDelete->setIcon(QIcon());
    }

    // Set Edit Action Menu Visibility
    ui->actionNew->setVisible(ctab == 1 || ctab == 2 || ctab == 3 || ctab == 4);
    ui->actionEdit->setVisible(ctab == 1 || ctab == 2 || ctab == 4);
    ui->actionCopy->setVisible(ctab == 1 || ctab == 4);
    ui->actionDelete->setVisible(ctab == 0 || ctab == 1 || ctab == 2 || ctab == 3 || ctab == 4);

    // Set Edit Action Menu enabled
    ui->actionNew->setEnabled(ctab == 1 || ctab == 2 || ctab == 3 || ctab == 4);
    ui->actionEdit->setEnabled(ctab == 1 || ctab == 2 || ctab == 4);
    ui->actionCopy->setEnabled(ctab == 1 || ctab == 4);
    ui->actionDelete->setEnabled(ctab == 0 || ctab == 1 || ctab == 2 || ctab == 3 || ctab == 4);

    /////////////////////////////////////////
    // Set Print Action Menu enabled
    ui->actionPrint->setEnabled(ctab == 0 || ctab == 1 || ctab == 4);
}

void SoftProjector::on_actionNew_triggered()
{
    int ctab = ui->projectTab->currentIndex();
    if(ctab == 1)
        newSong();
    else if(ctab == 2)
        newSlideShow();
    else if(ctab == 3)
        addMediaToLibrary();
    else if(ctab == 4)
        newAnnouncement();
}

void SoftProjector::on_actionEdit_triggered()
{
    int ctab = ui->projectTab->currentIndex();
    if(ctab == 1)
        editSong();
    else if(ctab == 2)
        editSlideShow();
    else if(ctab == 4)
        editAnnouncement();
}

void SoftProjector::on_actionCopy_triggered()
{
    int ctab = ui->projectTab->currentIndex();
    if(ctab == 1)
        copySong();
    else if(ctab ==4)
        copyAnnouncement();
}

void SoftProjector::on_actionDelete_triggered()
{
    int ctab = ui->projectTab->currentIndex();
    if(ctab == 0)
        bibleWidget->clearHistory();
    else if(ctab == 1)
        deleteSong();
    else if(ctab == 2)
        deleteSlideShow();
    else if(ctab == 3)
        removeMediaFromLibrary();
    else if(ctab == 4)
        deleteAnnoucement();
}

void SoftProjector::on_actionManage_Database_triggered()
{
    QSqlQuery sq;

    manageDialog->loadThemes();
    manageDialog->load_songbooks();
    manageDialog->exec();

    // Reload songbooks if Songbook has been added, edited, or deleted
    if (manageDialog->reload_songbook)
        songWidget->updateSongbooks();

    // Relaod themes if a theme has been deleted
    if (manageDialog->reloadThemes)
    {
        // Check if current theme has been deleted
        sq.exec("SELECT * FROM Themes WHERE is = " + theme.getThemeId());
        if(!sq.first())
        {
            GeneralSettings g = mySettings.general;
            Theme t;
            sq.exec("SELECT id FROM Themes");
            sq.first();
            t.setThemeId(sq.value(0).toInt());
            t.loadTheme();
            g.currentThemeId = t.getThemeId();
            updateSetting(g,t,mySettings.bibleSets,mySettings.bibleSets2);
            updateScreen();
        }
    }

    // Reload Bibles if Bible has been deleted
    if (manageDialog->reload_bible)
    {
        // check if Primary bible has been removed
        sq.exec("SELECT * FROM BibleVersions WHERE id = " + mySettings.bibleSets.primaryBible);
        if (!sq.first())
        {
            // If original primary bible has been removed, set first bible in the list to be primary
            sq.clear();
            sq.exec("SELECT id FROM BibleVersions");
            sq.first();
            mySettings.bibleSets.primaryBible = sq.value(0).toString();
        }
        sq.clear();

        // check if secondary bible has been removed, if yes, set secondary to "none"
        sq.exec("SELECT * FROM BibleVersions WHERE id = " + mySettings.bibleSets.secondaryBible);
        if (!sq.first())
            mySettings.bibleSets.secondaryBible = "none";
        sq.clear();

        // check if trinary bible has been removed, if yes, set secondary to "none"
        sq.exec("SELECT * FROM BibleVersions WHERE id = " + mySettings.bibleSets.trinaryBible);
        if (!sq.first())
            mySettings.bibleSets.trinaryBible = "none";
        sq.clear();

        // check if operator bible has been removed, if yes, set secondary to "same"
        sq.exec("SELECT * FROM BibleVersions WHERE id = " + mySettings.bibleSets.operatorBible);
        if (!sq.first())
            mySettings.bibleSets.operatorBible = "same";
        bibleWidget->setSettings(mySettings.bibleSets);
    }
}

void SoftProjector::on_actionDonate_triggered()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=FPCLPXFMH9XP4"));
}

void SoftProjector::on_actionAbout_triggered()
{
    AboutDialog *aboutDialog;
    aboutDialog = new AboutDialog(this, version_string);
    aboutDialog->exec();
    delete aboutDialog;
}

void SoftProjector::on_action_Help_triggered()
{
    helpDialog->show();
}

void SoftProjector::newSong()
{
    if (!editWidget->isHidden()) //Prohibits editing a song when a different song already been edited.
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("Cannot create a new song"));
        ms.setText(tr("Another song is already been edited."));
        ms.setInformativeText(tr("Please save and/or close current edited song before edited a different song."));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
    else
    {
        editWidget->show();
        editWidget->setNew();
        editWidget->activateWindow();
    }
}

void SoftProjector::editSong()
{
    if (songWidget->isSongSelected())
    {
        if(!editWidget->isHidden()) //Prohibits editing a song when a different song already been edited.
        {
            QMessageBox ms;
            ms.setWindowTitle(tr("Cannot start new edit"));
            ms.setText(tr("Another song is already been edited."));
            ms.setInformativeText(tr("Please save and/or close current edited song before edited a different song."));
            ms.setIcon(QMessageBox::Information);
            ms.exec();
        }
        else
        {
            editWidget->show();
            editWidget->setEdit(songWidget->getSongToEdit());
            editWidget->activateWindow();
        }
    }
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No song selected"));
        ms.setText(tr("No song has been selected to edit."));
        ms.setInformativeText(tr("Please select a song to edit."));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::copySong()
{
    if (songWidget->isSongSelected())
    {if (!editWidget->isHidden()) //Prohibits editing a song when a different song already been edited.
        {
            QMessageBox ms;
            ms.setWindowTitle(tr("Cannot copy this song"));
            ms.setText(tr("Another song is already been edited."));
            ms.setInformativeText(tr("Please save and/or close current edited song before edited a different song."));
            ms.setIcon(QMessageBox::Information);
            ms.exec();
        }
        else
        {
            editWidget->show();
            editWidget->setCopy(songWidget->getSongToEdit());
            editWidget->activateWindow();
        }
    }
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No song selected"));
        ms.setText(tr("No song has been selected to copy"));
        ms.setInformativeText(tr("Please select a song to copy"));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::deleteSong()
{
    if (songWidget->isSongSelected())
    {
        QString song_title = songWidget->currentSong().title;
        QMessageBox ms;
        ms.setWindowTitle(tr("Delete song?"));
        ms.setText(tr("Delete song \"") + song_title + "\"?");
        ms.setInformativeText(tr("This action will permanentrly delete this song"));
        ms.setIcon(QMessageBox::Question);
        ms.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ms.setDefaultButton(QMessageBox::Yes);
        int ret = ms.exec();

        switch (ret) {
        case QMessageBox::Yes:
            // Delete a song
            //        close();
            songWidget->deleteSong();
            break;
        case QMessageBox::No:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }
    }
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No song selected"));
        ms.setText(tr("No song has been selected to delete"));
        ms.setInformativeText(tr("Please select a song to delete"));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::newSlideShow()
{
    SlideShowEditor * sse = new SlideShowEditor;
    sse->exec();
    pictureWidget->loadSlideShows();
    delete sse;
}

void SoftProjector::editSlideShow()
{
    if(pictureWidget->isSlideShowSelected())
    {
        SlideShowEditor * sse = new SlideShowEditor;
        sse->setSlideShow(pictureWidget->getCurrentSlideshow());
        sse->exec();
        pictureWidget->loadSlideShows();
        delete sse;
    }
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No slideshow selected"));
        ms.setText(tr("No slideshow has been selected to edit."));
        ms.setInformativeText(tr("Please select a slideshow to edit."));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::deleteSlideShow()
{
    if(pictureWidget->isSlideShowSelected())
    {
        QString title = pictureWidget->getCurrentSlideshow().name;
        QMessageBox ms;
        ms.setWindowTitle(tr("Delete slideshow?"));
        ms.setText(tr("Delete slideshow: \"") + title + "\"?");
        ms.setInformativeText(tr("This action will permanentrly delete this slideshow"));
        ms.setIcon(QMessageBox::Question);
        ms.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ms.setDefaultButton(QMessageBox::Yes);
        int ret = ms.exec();

        switch (ret) {
        case QMessageBox::Yes:
            // Delete a slideshow
            pictureWidget->deleteSlideShow();
            break;
        case QMessageBox::No:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }
    }
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No slideshow selected"));
        ms.setText(tr("No slideshow has been selected to delete."));
        ms.setInformativeText(tr("Please select a slideshow to delete."));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::addMediaToLibrary()
{
    mediaPlayer->addToLibrary();
}

void SoftProjector::removeMediaFromLibrary()
{
    mediaPlayer->removeFromLibrary();
}

void SoftProjector::newAnnouncement()
{
    announceWidget->newAnnouncement();
}

void SoftProjector::editAnnouncement()
{
    if(announceWidget->isAnnounceValid())
        announceWidget->editAnnouncement();
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No Announcement Selected"));
        ms.setText(tr("No announcement has been selected to edit"));
        ms.setInformativeText(tr("Please select an announcement to edit"));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::copyAnnouncement()
{
    if(announceWidget->isAnnounceValid())
        announceWidget->copyAnnouncement();
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No Announcement Selected"));
        ms.setText(tr("No announcement has been selected to copy"));
        ms.setInformativeText(tr("Please select an announcement to copy"));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::deleteAnnoucement()
{
    if(announceWidget->isAnnounceValid())
    {
        QString title = announceWidget->getAnnouncement().title;
        QMessageBox ms;
        ms.setWindowTitle(tr("Delete Announcement?"));
        ms.setText(tr("Delete announcement: \"") + title + "\"?");
        ms.setInformativeText(tr("This action will permanentrly delete this announcement"));
        ms.setIcon(QMessageBox::Question);
        ms.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ms.setDefaultButton(QMessageBox::Yes);
        int ret = ms.exec();

        switch (ret) {
        case QMessageBox::Yes:
            // Delete a announce
            announceWidget->deleteAnnouncement();
            break;
        case QMessageBox::No:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }
    }
    else
    {
        QMessageBox ms;
        ms.setWindowTitle(tr("No Announcement Selected"));
        ms.setText(tr("No announcement has been selected to delete"));
        ms.setInformativeText(tr("Please select an announcement to delete"));
        ms.setIcon(QMessageBox::Information);
        ms.exec();
    }
}

void SoftProjector::setArrowCursor()
{
    this->setCursor(Qt::ArrowCursor);
}

void SoftProjector::setWaitCursor()
{
    this->setCursor(Qt::WaitCursor);
}

void SoftProjector::createLanguageActions()
{
    // find all *.qm files at language folder
     // and create coresponding action in language menu

        languageGroup = new QActionGroup(this);
        //default language and flag
        QAction *englishAction = new QAction(QIcon(":/icons/icons/flag_uk.png"), "English", this);
        englishAction->setCheckable(true);
        englishAction->setChecked(true);
        englishAction->setIconVisibleInMenu(true);
        languageGroup->addAction(englishAction);
        ui->menuLanguage->addAction(englishAction);

        QDir languageDir(languagePath);
        //all available languages
        QStringList languagesList = languageDir.entryList(QStringList("softpro_*.qm"), QDir::Files);
        //all available flags
        QStringList flagsList = languageDir.entryList(QStringList("flag_*.png"), QDir::Files);

        foreach(QString agent, languagesList)
        {
            // local translator for taken original language's name
            QTranslator tmpTranslator;
            tmpTranslator.load(agent, languageDir.absolutePath());
            // this string are used for detection language' name
            // this is one of translated strings

            QString fullLanguageName = tmpTranslator.translate("Native language name", "English","Do not change");
            QAction *tmpAction = new QAction(fullLanguageName, this);

            QString splocale = agent.remove(0, agent.indexOf('_')+1);
            splocale.chop(3);

            // flag's file name
            QString flagFileName = "flag_"+splocale+".png";
            if(flagsList.contains(flagFileName))//  if flag is available
            {
                tmpAction->setIcon(QIcon(languageDir.absolutePath() + QDir::separator() + flagFileName));
                tmpAction->setIconVisibleInMenu(true);
            }

            tmpAction->setData(splocale);// information abount localization
            tmpAction->setCheckable(true);
            languageGroup->addAction(tmpAction);
            ui->menuLanguage->addAction(tmpAction);
        }
//        connect(languageGroup, SIGNAL(triggered(QAction*)), this, SLOT(switchLanguage(QAction*)));
}

void SoftProjector::switchLanguage(QAction *action)
{
    cur_locale = action->data().toString();
    retranslateUis();
}

void SoftProjector::retranslateUis()
{
    qApp->removeTranslator(&translator);
    if(cur_locale != "en")
    {
        translator.load("softpro_"+cur_locale+".qm", QDir(languagePath).absolutePath());
        // qt libs translator must be add there,
        // but where are qt_locale.qm files?
        qApp->installTranslator(&translator);
    }

    ui->retranslateUi(this);
    ui->projectTab->setTabText(0, tr("Bible (F6)"));
    ui->projectTab->setTabText(1, tr("Songs (F7)"));
    ui->projectTab->setTabText(2, tr("Pictures"));
    ui->projectTab->setTabText(3, tr("Media"));
    ui->projectTab->setTabText(4, tr("Announcements (F8)"));
    songWidget->retranslateUis();
    editWidget->retranslateUis();
}

void SoftProjector::on_actionSong_Counter_triggered()
{
    SongCounter *songCounter;
    songCounter = new SongCounter(this, cur_locale);
    songCounter->exec();
    delete songCounter;
}

void SoftProjector::updateWindowText()
{
    QFileInfo fi(schedule_file_path);
    QString file = fi.fileName();

    if(!file.isEmpty())
    {
        file.remove(".spsc");
        if(is_schedule_saved)
            this->setWindowTitle(file + " - softProjector " + version_string);
        else
            this->setWindowTitle(file + "* - softProjector " + version_string);
    }
    else
    {
        this->setWindowTitle("softProjector " + version_string);
    }
}

void SoftProjector::on_rbMultiVerse_toggled(bool checked)
{
    if(checked)
        ui->listShow->setSelectionMode(QAbstractItemView::ContiguousSelection);
    else
        ui->listShow->setSelectionMode(QAbstractItemView::SingleSelection);
}

void SoftProjector::on_actionPrint_triggered()
{
    PrintPreviewDialog* p;
    p = new PrintPreviewDialog(this);
    if(ui->projectTab->currentIndex() == 0)
    {
        p->setText(mySettings.bibleSets.operatorBible + "," + mySettings.bibleSets.primaryBible,
                   bibleWidget->getCurrentBook(),bibleWidget->getCurrentChapter());
        p->exec();
    }
    else if (ui->projectTab->currentIndex() == 1)
    {
        if (songWidget->isSongSelected())
        {
            p->setText(songWidget->getSongToEdit());
            p->exec();

        }
        else
        {
            QMessageBox ms;
            ms.setWindowTitle(tr("No song selected"));
            ms.setText(tr("No song has been selected to be printed."));
            ms.setInformativeText(tr("Please select a song to be printed."));
            ms.setIcon(QMessageBox::Information);
            ms.exec();
        }
    }
    else if (ui->projectTab->currentIndex() == 4)
    {
        if(announceWidget->isAnnounceValid())
        {
            p->setText(announceWidget->getAnnouncement());
            p->exec();
        }
        else
        {
            QMessageBox ms;
            ms.setWindowTitle(tr("No announcement selected"));
            ms.setText(tr("No announcement has been selected to be printed."));
            ms.setInformativeText(tr("Please select a announcement to be printed."));
            ms.setIcon(QMessageBox::Information);
            ms.exec();
        }
    }
    delete p;
}

void SoftProjector::on_actionPrintSchedule_triggered()
{
    PrintPreviewDialog* p = new PrintPreviewDialog(this);
    p->setSchedule(schedule_file_path,schedule,false);
    p->exec();
    delete p;
}

void SoftProjector::nextSlide()
{
    // selects next item in the show list
    int current = ui->listShow->currentRow();
    if(ui->rbMultiVerse->isChecked())
    {
        // if multiple is selected, select last one
        for (int i(0);i<ui->listShow->count();++i)
        {
            if(ui->listShow->item(i)->isSelected())
                current = i;
        }
        if(current < ui->listShow->count()-1)
            ui->rbMultiVerse->setChecked(false);
    }
    if(current < ui->listShow->count()-1)
        ui->listShow->setCurrentRow(current+1);
}

void SoftProjector::prevSlide()
{
    // selects previous item in the show list
    int current = ui->listShow->currentRow();
    if(ui->rbMultiVerse->isChecked())
    {
        // if multiple is selected, select first one
        for (int i(0);i<ui->listShow->count();++i)
        {
            if(ui->listShow->item(i)->isSelected())
            {
                current = i;
                break;
            }
        }
        if(current>0)
            ui->rbMultiVerse->setChecked(false);
    }
    if(current>0)
        ui->listShow->setCurrentRow(current-1);
}

void SoftProjector::on_pushButtonPlay_clicked()
{
    if(displayScreen1->videoPlayer->state() == Phonon::PlayingState)
    {
        displayScreen1->videoPlayer->pause();
        if(hasDisplayScreen2)
            displayScreen2->videoPlayer->pause();
    }
    else
    {
        if(displayScreen1->videoPlayer->currentTime() == displayScreen1->videoPlayer->totalTime())
        {
            displayScreen1->videoPlayer->seek(0);
            if(hasDisplayScreen2)
                displayScreen2->videoPlayer->seek(0);
        }
        displayScreen1->videoPlayer->play();
        if(hasDisplayScreen2)
            displayScreen2->videoPlayer->play();
    }
}

void SoftProjector::setButtonPlayIcon(bool isPlaying)
{
    if (isPlaying)
        ui->pushButtonPlay->setIcon(QIcon(":icons/icons/pause.png"));
    else
        ui->pushButtonPlay->setIcon(QIcon(":icons/icons/play.png"));

}

void SoftProjector::setTimeText(QString cTime)
{
    ui->labelTime->setText(cTime);
}

void SoftProjector::on_actionScheduleAdd_triggered()
{
    int ctab = ui->projectTab->currentIndex();
    if(ctab == 0) // Bible
    {
        BibleHistory b = bibleWidget->getCurrentVerse();
        addToShcedule(b);
    }
    else if(ctab == 1) // Song
    {
        Song s = songWidget->getSongToEdit();
        addToShcedule(s);
    }
    else if(ctab == 2) // Slide Show
    {
        SlideShow ssi = pictureWidget->getCurrentSlideshow();
        addToShcedule(ssi);
    }
    else if(ctab == 3) // Multimedia
    {
        VideoInfo v = mediaPlayer->getMedia();
        addToShcedule(v);
    }
    else if(ctab == 4)
    {
        Announcement a = announceWidget->getAnnouncement();
        addToShcedule(a);
    }
    int row_count = ui->listWidgetSchedule->count();
    ui->listWidgetSchedule->setCurrentRow(row_count-1);
}

void SoftProjector::on_actionScheduleRemove_triggered()
{
    int cRow = ui->listWidgetSchedule->currentRow();
    if(cRow >=0)
    {
        schedule.takeAt(cRow);
        reloadShceduleList();
    }
}

void SoftProjector::on_actionScheduleClear_triggered()
{
    schedule.clear();
    reloadShceduleList();
}

void SoftProjector::addToShcedule(BibleHistory &b)
{
    Schedule d(b);
    schedule.append(d);
    reloadShceduleList();
}

void SoftProjector::addToShcedule(Song &s)
{
    Schedule d(s);
    schedule.append(d);
    reloadShceduleList();
}

void SoftProjector::addToShcedule(SlideShow &s)
{
    Schedule d(s);
    schedule.append(d);
    reloadShceduleList();
}

void SoftProjector::addToShcedule(VideoInfo &v)
{
    Schedule d(v);
    schedule.append(d);
    reloadShceduleList();
}

void SoftProjector::addToShcedule(Announcement &a)
{
    Schedule d(a);
    schedule.append(d);
    reloadShceduleList();
}

void SoftProjector::reloadShceduleList()
{
    ui->listWidgetSchedule->setCurrentRow(-1);
//    ui->listWidgetSchedule->clearSelection();
    ui->listWidgetSchedule->clear();
    foreach (const Schedule &s, schedule)
    {
        QListWidgetItem *itm = new QListWidgetItem;
        itm->setIcon(s.icon);
        itm->setText(s.name);
        ui->listWidgetSchedule->addItem(itm);
    }
    is_schedule_saved = false;
    updateWindowText();
}

void SoftProjector::on_listWidgetSchedule_doubleClicked(const QModelIndex &index)
{
    Schedule s = schedule.at(index.row());
    if(s.stype == "bible")
    {
        ui->projectTab->setCurrentIndex(0);
        bibleWidget->setSelectedHistory(s.bible);
        bibleWidget->sendToProjector(true);
    }
    else if(s.stype == "song")
    {
        ui->projectTab->setCurrentIndex(1);
        songWidget->sendToPreviewFromSchedule(s.song);
        setSongList(s.song,0);
        songWidget->counter.addSongCount(s.song);
//        songWidget->sendToProjector(s.song,0);
    }
    else if(s.stype == "slideshow")
    {
        ui->projectTab->setCurrentIndex(2);
        pictureWidget->sendToPreviewFromSchedule(s.slideshow);
        setPictureList(s.slideshow.slides,0);
    }
    else if(s.stype == "media")
    {
        ui->projectTab->setCurrentIndex(3);
        mediaPlayer->setMediaFromSchedule(s.media);
        mediaPlayer->goLiveFromSchedule();
    }
    else if(s.stype == "announce")
    {
        ui->projectTab->setCurrentIndex(4);
        announceWidget->setAnnouncementFromHistory(s.announce);
        setAnnounceText(s.announce,0);
    }
}

void SoftProjector::on_listWidgetSchedule_itemSelectionChanged()
{
    int currentRow = ui->listWidgetSchedule->currentRow();
    //qDebug()<<"cr:"<<currentRow;
    if(currentRow>=0)
    {
        Schedule s = schedule.at(currentRow);
        if(s.stype == "bible")
        {
            ui->projectTab->setCurrentIndex(0);
            bibleWidget->setSelectedHistory(s.bible);
        }
        else if(s.stype == "song")
        {
            ui->projectTab->setCurrentIndex(1);
            songWidget->sendToPreviewFromSchedule(s.song);
        }
        else if(s.stype == "slideshow")
        {
            ui->projectTab->setCurrentIndex(2);
            pictureWidget->sendToPreviewFromSchedule(s.slideshow);
        }
        else if(s.stype == "media")
        {
            ui->projectTab->setCurrentIndex(3);
            mediaPlayer->setMediaFromSchedule(s.media);
        }
        else if(s.stype == "announce")
        {
            ui->projectTab->setCurrentIndex(4);
            announceWidget->setAnnouncementFromHistory(s.announce);
        }
    }
}

void SoftProjector::on_actionMoveScheduleTop_triggered()
{
    int cs = ui->listWidgetSchedule->currentRow();
    if(cs>0)
    {
        schedule.move(cs,0);
        reloadShceduleList();
        ui->listWidgetSchedule->setCurrentRow(0);
    }
}

void SoftProjector::on_actionMoveScheduleUp_triggered()
{
    int cs = ui->listWidgetSchedule->currentRow();
    if(cs>0)
    {
        schedule.move(cs,cs-1);
        reloadShceduleList();
        ui->listWidgetSchedule->setCurrentRow(cs-1);
    }
}

void SoftProjector::on_actionMoveScheduleDown_triggered()
{
    int cs = ui->listWidgetSchedule->currentRow();
    int max = schedule.count();
    if(cs>=0 && cs<max-1)
    {
        schedule.move(cs,cs+1);
        reloadShceduleList();
        ui->listWidgetSchedule->setCurrentRow(cs+1);
    }
}

void SoftProjector::on_actionMoveScheduleBottom_triggered()
{
    int cs = ui->listWidgetSchedule->currentRow();
    int max = schedule.count();
    if(cs>=0 && cs<max-1)
    {
        schedule.move(cs,max-1);
        reloadShceduleList();
        ui->listWidgetSchedule->setCurrentRow(max-1);
    }
}

void SoftProjector::on_actionNewSchedule_triggered()
{
    if(!is_schedule_saved && !schedule_file_path.isEmpty())
    {
        QMessageBox mb;
        mb.setWindowTitle(tr("Save Schedule?"));
        mb.setText(tr("Do you want to save current schedule before creating a new schedule?"));
        mb.setIcon(QMessageBox::Question);
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Yes);
        int ret = mb.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
            // Yes to save unsaved schedule and contiue creating new schedule
            on_actionSaveSchedule_triggered();
            break;
        case QMessageBox::No:
            // No to save unsaved schedule and continue to create new schedule
            break;
        case QMessageBox::Cancel:
            // Cancel all
            return;
        default:
            // should never be reached
            break;
        }
    }

    schedule_file_path = "untitled.spsc";
    schedule.clear();
    reloadShceduleList();
    is_schedule_saved = false;
    updateWindowText();
}

void SoftProjector::on_actionOpenSchedule_triggered()
{
    if(!is_schedule_saved && !schedule_file_path.isEmpty())
    {
        QMessageBox mb;
        mb.setWindowTitle(tr("Save Schedule?"));
        mb.setText(tr("Do you want to save current schedule before opening a new schedule?"));
        mb.setIcon(QMessageBox::Question);
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Yes);
        int ret = mb.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
            // Yes to save unsaved schedule and contiue opening new schedule
            on_actionSaveSchedule_triggered();
            break;
        case QMessageBox::No:
            // No to save unsaved schedule and continue opening new schedule
            break;
        case QMessageBox::Cancel:
            // Cancel all
            return;
        default:
            // should never be reached
            break;
        }
    }

    QString path = QFileDialog::getOpenFileName(this,tr("Open softProjector schedule:"),".",
                                                tr("softProjector schedule file ") + "(*.spsc)");
    if(!path.isEmpty())
    {
        schedule_file_path = path;
        openSchedule();
        is_schedule_saved = true;
        updateWindowText();
    }
}

void SoftProjector::on_actionSaveSchedule_triggered()
{
    if(schedule_file_path.isEmpty() || schedule_file_path.startsWith("untitled"))
        on_actionSaveScheduleAs_triggered();
    else
        saveSchedule(false);
    updateWindowText();
}

void SoftProjector::on_actionSaveScheduleAs_triggered()
{
    QString path = QFileDialog::getSaveFileName(this,tr("Save softProjector schedule as:"),".",
                                                tr("softProjector schedule file ") + "(*.spsc)");
    if(!path.isEmpty())
    {
        if(path.endsWith(".spsc"))
            schedule_file_path = path;
        else
            schedule_file_path = path + ".spsc";
        saveSchedule(true);
    }

    updateWindowText();
}

void SoftProjector::on_actionCloseSchedule_triggered()
{
    if(!is_schedule_saved && !schedule_file_path.isEmpty())
    {
        QMessageBox mb;
        mb.setWindowTitle(tr("Save Schedule?"));
        mb.setText(tr("Do you want to save current schedule before closing it?"));
        mb.setIcon(QMessageBox::Question);
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Yes);
        int ret = mb.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
            // Yes to save unsaved schedule and contiue creating new schedule
            on_actionSaveSchedule_triggered();
            break;
        case QMessageBox::No:
            // No to save unsaved schedule and continue to create new schedule
            break;
        case QMessageBox::Cancel:
            // Cancel all
            return;
        default:
            // should never be reached
            break;
        }
    }

    schedule_file_path.clear();
    schedule.clear();
    reloadShceduleList();
    is_schedule_saved = true;
    updateWindowText();
}

void SoftProjector::saveSchedule(bool overWrite)
{
    // Save schedule as s SQLite database file
    QProgressDialog progress;
    progress.setMaximum(0);
    progress.setLabelText(tr("Saving schedule file..."));
    progress.show();
    {
        bool db_exist = QFile::exists(schedule_file_path);
        if(db_exist && overWrite)
        {
            if(!QFile::remove(schedule_file_path))
            {
                QMessageBox mb;
                mb.setText(tr("An error has ocured when overwriting existing file.\n"
                              "Please try again with different file name"));
                mb.setIcon(QMessageBox::Information);
                mb.setStandardButtons(QMessageBox::Ok);
                mb.exec();
                return;
            }
            else
                db_exist = false;
        }
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","spsc");
        db.setDatabaseName(schedule_file_path);
        if(db.open())
        {
            QSqlQuery sq(db);
            sq.exec("PRAGMA user_version = 2");
            sq.exec("CREATE TABLE IF NOT EXISTS 'schedule' ('id' INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL, "
                    "'stype' TEXT, 'name' TEXT, 'sorder' INTEGER )");
            sq.exec("CREATE TABLE IF NOT EXISTS 'bible' ('scid' INTEGER, 'verseIds' TEXT, 'caption' TEXT, 'captionLong' TEXT)");
            sq.exec("CREATE TABLE IF NOT EXISTS 'song' ('scid' INTEGER, 'songid' INTEGER, 'sbid' INTEGER, 'sbName' TEXT, "
                    "'number' INTEGER, 'title' TEXT, 'category' INTEGER, 'tune' TEXT, 'wordsBy' TEXT, 'musicBy' TEXT, "
                    "'songText' TEXT, 'notes' TEXT, 'usePrivate' BOOL, 'alignV' INTEGER, 'alignH' INTEGER, 'color' INTEGER, "
                    "'font' TEXT, 'backImage' BLOB, 'backPath' TEXT)");
            sq.exec("CREATE TABLE IF NOT EXISTS 'slideshow' ('scid' INTEGER, 'ssid' INTEGER, 'name' TEXT, 'info' TEXT)");
            sq.exec("CREATE TABLE IF NOT EXISTS 'slides' ('scid' INTEGER, 'sid' INTEGER, 'name' TEXT, 'path' TEXT, "
                    "'porder' INTEGER, 'image' BLOB, 'imageSmall' BLOB, 'imagePreview' BLOB)");
            sq.exec("CREATE TABLE IF NOT EXISTS 'media' ('scid' INTEGER, 'name' TEXT, 'path' TEXT, 'aRatio' INTEGER)");
            sq.exec("CREATE TABLE IF NOT EXISTS 'announce' ('scid' INTEGER, 'aId' INTEGER, 'title' TEXT, 'aText' TEXT, "
                    "'usePrivate' BOOL, 'useAuto' BOOL, 'loop' BOOL, 'slideTimer' INTEGER, 'font' TEXT, 'color' INTEGER, "
                    "'useBack' BOOL, 'backImage' BLOB, 'backPath' TEXT, 'alignV' INTEGER, 'alignH' INTEGER)");
            if(db_exist)
                saveScheduleUpdate(sq);
            else
                saveScheduleNew(sq);
        }
    }
    QSqlDatabase::removeDatabase("spsc");
    is_schedule_saved = true;
    progress.close();
}

void SoftProjector::saveScheduleNew(QSqlQuery &q)
{
    for(int i(0);i < schedule.count();++i)
    {
        Schedule sc =  schedule.at(i);
        q.exec(QString("INSERT INTO schedule (stype,name,sorder) VALUES('%1','%2',%3)")
               .arg(sc.stype).arg(sc.name).arg(i+1));
        q.exec("SELECT seq FROM sqlite_sequence WHERE name = 'schedule'");
        q.first();
        sc.scid = q.value(0).toInt();
        q.clear();
        if(sc.stype == "bible")
            saveScheduleItemNew(q,sc.scid,sc.bible);
        else if(sc.stype == "song")
            saveScheduleItemNew(q,sc.scid,sc.song);
        else if(sc.stype == "slideshow")
            saveScheduleItemNew(q,sc.scid,sc.slideshow);
        else if(sc.stype == "media")
            saveScheduleItemNew(q,sc.scid,sc.media);
        else if(sc.stype == "announce")
            saveScheduleItemNew(q,sc.scid,sc.announce);
        schedule.replace(i,sc);
    }
}

void SoftProjector::saveScheduleItemNew(QSqlQuery &q, int scid, const BibleHistory &b)
{
    q.prepare("INSERT INTO bible (scid,verseIds,caption,captionLong) VALUES(?,?,?,?)");
    q.addBindValue(scid);
    q.addBindValue(b.verseIds);
    q.addBindValue(b.caption);
    q.addBindValue(b.captionLong);
    q.exec();
}

void SoftProjector::saveScheduleItemNew(QSqlQuery &q, int scid, const Song &s)
{
    q.prepare("INSERT INTO song (scid,songid,sbid,sbName,number,title,category,tune,wordsBy,musicBy,"
              "songText,notes,usePrivate,alignV,alignH,color,font,backImage,backPath) "
              "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    q.addBindValue(scid);
    q.addBindValue(s.songID);
    q.addBindValue(s.songbook_id);
    q.addBindValue(s.songbook_name);
    q.addBindValue(s.number);
    q.addBindValue(s.title);
    q.addBindValue(s.category);
    q.addBindValue(s.tune);
    q.addBindValue(s.wordsBy);
    q.addBindValue(s.musicBy);
    q.addBindValue(s.songText);
    q.addBindValue(s.notes);
    q.addBindValue(s.usePrivateSettings);
    q.addBindValue(s.alignmentV);
    q.addBindValue(s.alignmentH);
    unsigned int tci = (unsigned int)(s.color.rgb());
    q.addBindValue(tci);
    q.addBindValue(s.font.toString());
    q.addBindValue(pixToByte(s.background));
    q.addBindValue(s.backgroundName);
    q.exec();
}

void SoftProjector::saveScheduleItemNew(QSqlQuery &q, int scid, const SlideShow &s)
{
    q.prepare("INSERT INTO slideshow (scid,ssid,name,info) VALUES (?,?,?,?)");
    q.addBindValue(scid);
    q.addBindValue(s.slideShowId);
    q.addBindValue(s.name);
    q.addBindValue(s.info);
    q.exec();

    foreach(const SlideShowItem & si,s.slides)
    {
        q.prepare("INSERT INTO slides (scid,sid,name,path,porder,image,imageSmall,imagePreview) VALUES(?,?,?,?,?,?,?,?)");
        q.addBindValue(scid);
        q.addBindValue(si.slideId);
        q.addBindValue(si.name);
        q.addBindValue(si.path);
        q.addBindValue(si.order);
        q.addBindValue(pixToByte(si.image));
        q.addBindValue(pixToByte(si.imageSmall));
        q.addBindValue(pixToByte(si.imagePreview));
        q.exec();
    }
}

void SoftProjector::saveScheduleItemNew(QSqlQuery &q, int scid, const VideoInfo &v)
{
    q.prepare("INSERT INTO media (scid,name,path,aRatio) VALUES(?,?,?,?)");
    q.addBindValue(scid);
    q.addBindValue(v.fileName);
    q.addBindValue(v.filePath);
    q.addBindValue(v.aspectRatio);
    q.exec();
}

void SoftProjector::saveScheduleItemNew(QSqlQuery &q, int scid, const Announcement &a)
{
    q.prepare("INSERT INTO announce (scid,aId,title,aText,usePrivate,useAuto,loop,slideTimer,font,"
                        "color,useBack,backImage,backPath,alignV,alignH) "
                        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    q.addBindValue(scid);
    q.addBindValue(a.idNum);
    q.addBindValue(a.title);
    q.addBindValue(a.text);
    q.addBindValue(a.usePrivateSettings);
    q.addBindValue(a.useAutoNext);
    q.addBindValue(a.loop);
    q.addBindValue(a.slideTimer);
    q.addBindValue(a.font.toString());
    unsigned int tci = (unsigned int)(a.color.rgb());
    q.addBindValue(tci);
    q.addBindValue(a.useBackground);
    q.addBindValue(pixToByte(QPixmap()));
    q.addBindValue(a.backgroundPath);
    q.addBindValue(a.alignmentV);
    q.addBindValue(a.alignmentH);
    q.exec();
}

void SoftProjector::saveScheduleUpdate(QSqlQuery &q)
{
    for(int i(0);i < schedule.count();++i)
    {
        Schedule sc =  schedule.at(i);
        if(sc.scid == -1)   // Save new schedule item that was not saved yet
        {
            q.exec(QString("INSERT INTO schedule (stype,name,sorder) VALUES('%1','%2',%3)")
                   .arg(sc.stype).arg(sc.name).arg(i+1));
            q.exec("SELECT seq FROM sqlite_sequence WHERE name = 'schedule'");
            q.first();
            sc.scid = q.value(0).toInt();
            q.clear();
            if(sc.stype == "bible")
                saveScheduleItemNew(q,sc.scid,sc.bible);
            else if(sc.stype == "song")
                saveScheduleItemNew(q,sc.scid,sc.song);
            else if(sc.stype == "slideshow")
                saveScheduleItemNew(q,sc.scid,sc.slideshow);
            else if(sc.stype == "media")
                saveScheduleItemNew(q,sc.scid,sc.media);
            else if(sc.stype == "announce")
                saveScheduleItemNew(q,sc.scid,sc.announce);
            schedule.replace(i,sc);
        }
        else    // Update existing schedule item
        {
            q.exec(QString("UPDATE schedule SET sorder = %1 WHERE id = %2").arg(i+1).arg(sc.scid));
//            if(sc.stype == "bible")
//                saveScheduleItemUpdate(q,sc.scid,sc.bible);
//            else if(sc.stype == "song")
//                saveScheduleItemUpdate(q,sc.scid,sc.song);
//            else if(sc.stype == "slideshow")
//                saveScheduleItemUpdate(q,sc.scid,sc.slideshow);
//            else if(sc.stype == "media")
//                saveScheduleItemUpdate(q,sc.scid,sc.media);
//            else if(sc.stype == "announce")
//                saveScheduleItemUpdate(q,sc.scid,sc.announce);
        }
    }

    // Delete any shcedule items from file that have removed from schedule
    q.exec("SELECT id,stype FROM schedule");
    QSqlQuery sq = q;
    while(q.next())
    {
        int scid = q.value(0).toInt();
        bool toDelete;
        QString stype = q.value(1).toString();
        foreach(const Schedule &s,schedule)
        {
            if(scid == s.scid)
            {
                toDelete = false;
                break;
            }
            else
                toDelete = true;
        }

        if(toDelete)
        {
            sq.exec("DELETE FROM schedule WHERE id = " + QString::number(scid));
            sq.exec("DELETE FROM " + stype + " WHERE scid = " + QString::number(scid));
            if(stype == "slideshow")
                sq.exec("DELETE FROM slides WHERE scid = " + QString::number(scid));
        }
    }
}

void SoftProjector::saveScheduleItemUpdate(QSqlQuery &q, int scid, const BibleHistory &b)
{

}

void SoftProjector::saveScheduleItemUpdate(QSqlQuery &q, int scid, const Song &s)
{

}

void SoftProjector::saveScheduleItemUpdate(QSqlQuery &q, int scid, const SlideShow &s)
{

}

void SoftProjector::saveScheduleItemUpdate(QSqlQuery &q, int scid, const VideoInfo &v)
{

}

void SoftProjector::saveScheduleItemUpdate(QSqlQuery &q, int scid, const Announcement &a)
{

}

void SoftProjector::openSchedule()
{
    QProgressDialog progress;
    progress.setMaximum(0);
    progress.setLabelText(tr("Opening schedule file..."));
    progress.show();
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","spsc");
        db.setDatabaseName(schedule_file_path);
        if(db.open())
        {
            QSqlQuery sq(db);
            sq.exec("PRAGMA user_version");
            sq.first();
            int scVer = sq.value(0).toInt();
            if(scVer == 2)
            {
                schedule.clear();
                sq.exec("SELECT id, stype, name FROM schedule ORDER BY sorder");
                QSqlQuery sqsc = sq;
                while(sq.next())
                {
//                    Schedule sc;
                    int scid = sq.value(0).toInt();
                    QString stype = sq.value(1).toString();
                    QString name = sq.value(2).toString();

                    if(stype == "bible")
                    {
                        BibleHistory bib;
                        openScheduleItem(sqsc,scid,bib);
                        Schedule sc(bib);
                        sc.name = name;
                        sc.scid = scid;
                        schedule.append(sc);
                    }
                    else if(stype == "song")
                    {
                        Song song;
                        openScheduleItem(sqsc,scid,song);
                        Schedule sc(song);
                        sc.name = name;
                        sc.scid = scid;
                        schedule.append(sc);
                    }
                    else if(stype == "slideshow")
                    {
                        SlideShow ss;
                        openScheduleItem(sqsc,scid,ss);
                        Schedule sc(ss);
                        sc.name = name;
                        sc.scid = scid;
                        schedule.append(sc);
                    }
                    else if(stype == "media")
                    {
                        VideoInfo vi;
                        openScheduleItem(sqsc,scid,vi);
                        Schedule sc(vi);
                        sc.name = name;
                        sc.scid = scid;
                        schedule.append(sc);
                    }
                    else if(stype == "announce")
                    {
                        Announcement announce;
                        openScheduleItem(sqsc,scid,announce);
                        Schedule sc(announce);
                        sc.name = name;
                        sc.scid = scid;
                        schedule.append(sc);
                    }
                }
                reloadShceduleList();
            }
            else
            {
                QMessageBox mb;
                mb.setText(tr("The schedule file you are trying to open is of uncompatible version.\n"
                              "Compatible version: 2\n"
                              "This schedule file version: ") + QString::number(scVer));
                mb.setIcon(QMessageBox::Information);
                mb.setStandardButtons(QMessageBox::Ok);
                mb.exec();
                schedule_file_path.clear();
                updateWindowText();
            }
        }
    }
    QSqlDatabase::removeDatabase("spsc");
    progress.close();
}

void SoftProjector::openScheduleItem(QSqlQuery &q, const int scid, BibleHistory &b)
{
    q.exec("SELECT verseIds, caption, captionLong FROM bible WHERE scid = " + QString::number(scid));
    q.first();
    b.verseIds = q.value(0).toString();
    b.caption = q.value(1).toString();
    b.caption = q.value(2).toString();
}

void SoftProjector::openScheduleItem(QSqlQuery &q, const int scid, Song &s)
{
    q.exec("SELECT songid, sbid, sbName, number, title, category, tune, wordsBy, musicBy, songText, notes, usePrivate, "
           "alignV, alignH, color, font, backImage, backPath FROM song WHERE scid = " + QString::number(scid));
    q.first();
    s.songID = q.value(0).toInt();
    s.songbook_id = q.value(1).toInt();
    s.songbook_name = q.value(2).toString();
    s.number = q.value(3).toInt();
    s.title = q.value(4).toString();
    s.category = q.value(5).toInt();
    s.tune = q.value(6).toString();
    s.wordsBy = q.value(7).toString();
    s.musicBy = q.value(8).toString();
    s.songText = q.value(9).toString();
    s.notes = q.value(10).toString();
    s.usePrivateSettings = q.value(11).toBool();
    s.alignmentV = q.value(12).toInt();
    s.alignmentH = q.value(13).toInt();
    s.color = QColor::fromRgb(q.value(14).toUInt());
    s.font.fromString(q.value(15).toString());
    s.background.loadFromData(q.value(16).toByteArray());
    s.backgroundName = q.value(17).toString();
}

void SoftProjector::openScheduleItem(QSqlQuery &q, const int scid, SlideShow &s)
{
    q.exec("SELECT ssid, name, info FROM slideshow WHERE scid = " + QString::number(scid));
    q.first();
    s.slideShowId = q.value(0).toInt();
    s.name = q.value(1).toString();
    s.info = q.value(2).toString();

    q.exec("SELECT sid, name, path, porder, image, imageSmall, imagePreview FROM slides WHERE scid = " + QString::number(scid));
    while(q.next())
    {
        SlideShowItem si;
        si.slideId = q.value(0).toInt();
        si.name = q.value(1).toString();
        si.path = q.value(2).toString();
        si.order = q.value(3).toInt();
        si.image.loadFromData(q.value(4).toByteArray());
        si.imageSmall.loadFromData(q.value(5).toByteArray());
        si.imagePreview.loadFromData(q.value(6).toByteArray());
        s.slides.append(si);
    }
}

void SoftProjector::openScheduleItem(QSqlQuery &q, const int scid, VideoInfo &v)
{
    q.exec("SELECT name, path, aRatio FROM media WHERE scid = " + QString::number(scid));
    q.first();
    v.fileName = q.value(0).toString();
    v.filePath = q.value(1).toString();
    v.aspectRatio = q.value(2).toInt();
}

void SoftProjector::openScheduleItem(QSqlQuery &q, const int scid, Announcement &a)
{
    q.exec("SELECT aId, title, aText, usePrivate, useAuto, loop, slideTimer, font, color, useBack, "
           "backImage, backPath, alignV, alignH FROM announce WHERE scid = " + QString::number(scid));
    q.first();
    a.idNum = q.value(0).toInt();
    a.title = q.value(1).toString();
    a.text = q.value(2).toString();
    a.usePrivateSettings = q.value(3).toBool();
    a.useAutoNext = q.value(4).toBool();
    a.loop = q.value(5).toBool();
    a.slideTimer = q.value(6).toInt();
    a.font.fromString(q.value(7).toString());
    a.color = QColor::fromRgb(q.value(8).toUInt());
    a.useBackground = q.value(9).toBool();
//    a.backImage = QPixmap
    a.backgroundPath = q.value(11).toString();
    a.alignmentV = q.value(12).toInt();
    a.alignmentH = q.value(13).toInt();
}
