#ifndef BULLET_H
#define BULLET_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>

class QPainter;
class Enemy;
class MainWindow;

class Bullet :public QObject
{
    Q_OBJECT//所有应用QT槽的类都需要声明
	Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
	Bullet(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/bullet.png"));

	void draw(QPainter *painter) const;
	void move();
	void setCurrentPos(QPoint pos);
	QPoint currentPos() const;

public slots:
    virtual void hitTarget();

protected:
	const QPoint	m_startPos;
	const QPoint	m_targetPos;
	const QPixmap	m_sprite;
	QPoint			m_currentPos;
	Enemy *			m_target;
	MainWindow *	m_game;
	int				m_damage;

	static const QSize ms_fixedSize;
};

class BulletSlowingAttack:public Bullet
{
    Q_OBJECT//所有应用QT槽的类都需要声明
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
    BulletSlowingAttack(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/bulletSlow.png"));
    ~BulletSlowingAttack();

public slots:
    virtual void hitTarget();

};

class BulletStrongAttack:public Bullet
{
    Q_OBJECT//所有应用QT槽的类都需要声明
    Q_PROPERTY(QPoint m_currentPos READ currentPos WRITE setCurrentPos)

public:
    BulletStrongAttack(QPoint startPos, QPoint targetPoint, int damage, Enemy *target,
           MainWindow *game, const QPixmap &sprite = QPixmap(":/image/Bullet 2-1normal.png"));
    ~BulletStrongAttack();

};

#endif // BULLET_H
