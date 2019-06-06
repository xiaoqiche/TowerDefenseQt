#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>

class WayPoint;
class QPainter;
class MainWindow;
class Tower;
// 这个类是敌方类（怪兽类）的定义


class Enemy : public QObject
{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    Enemy(WayPoint *startWayPoint, MainWindow *game,
          const QPixmap &sprite = QPixmap(":/image/enemy_brightYellow.png"),
          int maxHp = 40,qreal walkingSpeed = 1.0);//血量和速度应该是可以更改的部分
	~Enemy();

	void draw(QPainter *painter) const;
	void move();
	void getDamage(int damage);
	void getRemoved();
	void getAttacked(Tower *attacker);
	void gotLostSight(Tower *attacker);
	QPoint pos() const;

public slots:
	void doActivate();

protected://为继承做准备
	bool			m_active;
	int				m_maxHp;
	int				m_currentHp;
	qreal			m_walkingSpeed;
	qreal			m_rotationSprite;

	QPoint			m_pos;
	WayPoint *		m_destinationWayPoint;
	MainWindow *	m_game;
	QList<Tower *>	m_attackedTowersList;

	const QPixmap	m_sprite;
	static const QSize ms_fixedSize;
};

#endif // ENEMY_H
