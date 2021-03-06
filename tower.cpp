#include "tower.h"
#include "enemy.h"
#include "bullet.h"
#include "mainwindow.h"
#include "utility.h"
#include <QPainter>
#include <QColor>
#include <QTimer>
#include <QVector2D>
#include <QtMath>

const QSize Tower::ms_fixedSize(42, 42);

Tower::Tower(QPoint pos, MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/tower.png"*/)
	: m_attacking(false)
	, m_attackRange(70)
	, m_damage(10)
	, m_fireRate(1000)
	, m_rotationSprite(0.0)
	, m_chooseEnemy(NULL)
	, m_game(game)
	, m_pos(pos)
	, m_sprite(sprite)
{
    m_towerType=1;
	m_fireRateTimer = new QTimer(this);
	connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

Tower::~Tower()
{
	delete m_fireRateTimer;
	m_fireRateTimer = NULL;
}

TowerSlowingAttack::TowerSlowingAttack(QPoint pos, MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/tower2.png"*/)
    : Tower(pos,game,sprite)
{
    m_towerType=2;
    m_attackRange=70;
    m_damage=10;
    m_fireRate=1000;
    m_fireRateTimer = new QTimer(this);
    connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

TowerSlowingAttack::~TowerSlowingAttack()
{
    delete m_fireRateTimer;
    m_fireRateTimer = NULL;
}


Tower2::Tower2(QPoint pos, MainWindow *game, const QPixmap &sprite/* = QPixmap(":/image/tower.png"*/)
    : Tower(pos,game,sprite)
{
    m_attackRange=100;
    m_damage=12;
    m_fireRate=800;
    m_fireRateTimer = new QTimer(this);
    connect(m_fireRateTimer, SIGNAL(timeout()), this, SLOT(shootWeapon()));
}

Tower2::~Tower2()
{
    delete m_fireRateTimer;
    m_fireRateTimer = NULL;
}

QPoint Tower::getPosition()
{
    return m_pos;
}

void Tower::setTowerLevel(int level)
{
    m_towerLevel=level;
    if(level==1)
    {
        m_towerType=1;
        m_attackRange=70;
        m_damage=10;
        m_fireRate=1000;
    }
    if(level==2)
    {
        //m_sprite=QPixmap(":/image/towerUpgrade1.png");
        m_attackRange=100;
        m_damage=12;
        m_fireRate=800;
    }
}

int Tower::getTowerLevel()
{
    return m_towerLevel;
}


void Tower::checkEnemyInRange()
{
	if (m_chooseEnemy)
	{
		// 这种情况下,需要旋转炮台对准敌人
		// 向量标准化
		QVector2D normalized(m_chooseEnemy->pos() - m_pos);
		normalized.normalize();
		m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) - 90;

		// 如果敌人脱离攻击范围
		if (!collisionWithCircle(m_pos, m_attackRange, m_chooseEnemy->pos(), 1))
			lostSightOfEnemy();
	}
	else
	{
		// 遍历敌人,看是否有敌人在攻击范围内
		QList<Enemy *> enemyList = m_game->enemyList();
		foreach (Enemy *enemy, enemyList)
		{
			if (collisionWithCircle(m_pos, m_attackRange, enemy->pos(), 1))
			{
				chooseEnemyForAttack(enemy);
				break;
			}
		}
	}
}

void Tower::draw(QPainter *painter) const
{
	painter->save();
	painter->setPen(Qt::white);
	// 绘制攻击范围
	painter->drawEllipse(m_pos, m_attackRange, m_attackRange);

	// 绘制偏转坐标,由中心+偏移=左上
	static const QPoint offsetPoint(-ms_fixedSize.width() / 2, -ms_fixedSize.height() / 2);
	// 绘制炮塔并选择炮塔
	painter->translate(m_pos);
	painter->rotate(m_rotationSprite);
	painter->drawPixmap(offsetPoint, m_sprite);
	painter->restore();
}

void Tower::attackEnemy()
{
	m_fireRateTimer->start(m_fireRate);
}

void Tower::chooseEnemyForAttack(Enemy *enemy)
{
	m_chooseEnemy = enemy;
	attackEnemy();
	m_chooseEnemy->getAttacked(this);
}

void Tower::shootWeapon()
{
    Bullet *bullet = new Bullet(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
	bullet->move();
	m_game->addBullet(bullet);
}

void Tower::targetKilled()
{
	if (m_chooseEnemy)
		m_chooseEnemy = NULL;

	m_fireRateTimer->stop();
	m_rotationSprite = 0.0;
}

void Tower::lostSightOfEnemy()
{
	m_chooseEnemy->gotLostSight(this);
	if (m_chooseEnemy)
		m_chooseEnemy = NULL;

	m_fireRateTimer->stop();
	m_rotationSprite = 0.0;
}

void TowerSlowingAttack::shootWeapon()
{
    Bullet *bullet = new BulletSlowingAttack(m_pos, m_chooseEnemy->pos(), m_damage, m_chooseEnemy, m_game);
    bullet->move();
    m_game->addBullet(bullet);
}

void TowerSlowingAttack::chooseEnemyForAttack(Enemy *enemy)
{
    m_chooseEnemy = enemy;
    attackEnemy();
    //enemy->slowDown();
    m_chooseEnemy->getAttacked(this);
}

int Tower::getTowerType()
{
    return m_towerType;
}
