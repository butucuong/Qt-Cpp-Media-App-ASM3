/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "player.h"

#include "playlistmodel.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QMediaMetaData>
#include <QObject>
#include <QFileInfo>
#include <QTime>
#include <QDir>
#include <QStandardPaths>

Player::Player(QObject *parent)
    : QObject(parent)
{
    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(this);
    m_player->setPlaylist(m_playlist);
    m_playlistModel = new PlaylistModel(this);
    open();
    if (!m_playlist->isEmpty()) {
        m_playlist->setCurrentIndex(0);
    }
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    m_player->play();

}

void Player::open()
{
    QDir directory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation)[0]);    //The directory of songs is taken from the Music location in computer
    QFileInfoList musics = directory.entryInfoList(QStringList() << "*.mp3",QDir::Files);   //Filtering the files which ending with .mp3 in list files
    QList<QUrl> urls;
    for (int i = 0; i < musics.length(); i++){
        urls.append(QUrl::fromLocalFile(musics[i].absoluteFilePath()));
    }
    addToPlaylist(urls);
}

void Player::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {

        m_playlist->addMedia(url);                                                          //Add the content item into the playlist

        const char * path=url.path().toStdString().c_str();                                 //Path pointer point to the relative path
        path+=1;
        FileRef f(path);                                                                    //FileRef take the ownership of the pointer and will free the pointer when it out of scope
                                            //FileRef provide the method to handling files, be able to get or set some of tag information accross file types

        //FileRef f(url.path().toStdString().c_str());
        Tag *tag = f.tag();                                                                 //Return the pointer represent the file's tag
        if(tag){									//Use function of class Tag to get tag's name
            Song song(QString::fromWCharArray(tag->title().toCWString()),
                     QString::fromWCharArray(tag->artist().toCWString()), url.toDisplayString(),
                      getAlbumArt(dynamic_cast<TagLib::MPEG::File*>(f.file())));	//Casting from the file pointer to An MPEG file class with some useful methods specific to MPEG
            m_playlistModel->addSong(song);
        }
    }
}

QString Player::getTimeInfo(qint64 currentInfo)
{
    QString tStr = "00:00";
    currentInfo = currentInfo/1000;
    qint64 durarion = m_player->duration()/1000;
    if (currentInfo || durarion) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                          currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((durarion / 3600) % 60, (m_player->duration() / 60) % 60,
                        durarion % 60, (m_player->duration() * 1000) % 1000);
        QString format = "mm:ss";
        if (durarion > 3600)
            format = "hh::mm:ss";
        tStr = currentTime.toString(format);
    }
    return tStr;
}

QString Player::getAlbumArt(MPEG::File *mpegFile)
{
    static const char *IdPicture = "APIC" ;

    TagLib::ID3v2::Tag *id3v2tag = mpegFile->ID3v2Tag(); //Creating ID3v2 tag for the file, return true if creatabel
    TagLib::ID3v2::FrameList Frame ;			 //Creat the list of frames
    TagLib::ID3v2::AttachedPictureFrame *PicFrame ;	 //Pictures may be included in tags, one per APIC frame, These pictures are usually in either JPEG or PNG format.
    void *SrcImage;
    unsigned long Size;
    auto name = mpegFile->name().toString() + ".jpg";
    FILE *jpegFile;
    jpegFile = fopen(name.toCString(),"wb");		//Write the JPG file as the binary ('wb') and read all the content into CString

    if ( id3v2tag )
    {
        // picture frame
        Frame = id3v2tag->frameListMap()[IdPicture]; //looking for picture frame "APIC" only
        if (!Frame.isEmpty() )
        {
            for(TagLib::ID3v2::FrameList::ConstIterator it = Frame.begin(); it != Frame.end(); ++it)
            {
                PicFrame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(*it); //casting Frame* to AttachPictureFrame*
                {
                    // extract image (in it’s compressed form)
                    Size = PicFrame->picture().size() ;
                    SrcImage = malloc ( Size ) ;	//Allocate a block of Size bytes of memory, return the pointer beginning the block if allocated, return NULL if no data
                    if ( SrcImage )
                    {
                        memcpy ( SrcImage, PicFrame->picture().data(), Size ) ; //Copy the data into the SrcImage
                        fwrite(SrcImage,Size,1, jpegFile); //write the data from SrcImage to jpegFile
                        fclose(jpegFile);	//close the file
                        free( SrcImage ) ;	//free the memory
                        return QUrl::fromLocalFile(name.toCString()).toDisplayString();
                    }

                }
            }
        }
    }
    else
    {
        qDebug() <<"id3v2 not present";
        return "qrc:/Image/album_art.png";
    }
    return "qrc:/Image/album_art.png";
}
void Player::toggleRandom(){	//State transiton
    m_random = !m_random;
    updatePlaybackMode();
}

void Player::toggleLoop() {	//State transition
    m_loop = !m_loop;
    updatePlaybackMode();
}
void Player::updatePlaybackMode(){
    auto status = QMediaPlaylist::Loop;
    if(m_random)
        status =QMediaPlaylist::Random;
    if(m_loop)
        status =QMediaPlaylist::CurrentItemInLoop;
    m_playlist-> setPlaybackMode(status);
}

void Player::next(){
    if(m_playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){
        if(m_random){
            m_playlist->setPlaybackMode(QMediaPlaylist::Random);

        }else{
            m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
        }
        m_playlist->next();
        m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }else{
        m_playlist->next();
    }
}

void Player::prev(){
    if(m_playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){
        if(m_random){
            m_playlist->setPlaybackMode(QMediaPlaylist::Random);

        }else{
            m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
        }
        m_playlist->previous();
        m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }else{
        m_playlist->previous();
    }
}
