#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>

class QMediaPlayer;

enum SoundType
{
    TowerPlaceSound,		// 放塔时的声音
    LaserShootSound,		// 打中敌人时的声音
    EnemyDestorySound		// 怪兽消失时的声音
};

class AudioPlayer : public QObject
{
public:
    explicit AudioPlayer(QObject *parent = 0);

    void startBGM();
    void playSound(SoundType soundType);

private:
    QMediaPlayer *m_backgroundMusic; // 只用来播放背景音乐
};


#endif // AUDIOPLAYER_H
