#include "enemy.h"
#include "waypoint.h"
#include "tower.h"
#include "utility.h"
#include "mainwindow.h"
#include "audioplayer.h"
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QMatrix>
#include <QVector2D>
#include <QtMath>
// 这个类是敌方类（怪兽类）的实现
static const int Health_Bar_Width = 20;

const QSize Enemy::ms_fixedSize(52, 52);

Enemy::Enemy(WayPoint *startWayPoint, MainWindow *game,
             const QPixmap &sprite/* = QPixmap(":/image/enemy.png")*/,
             int maxHp/* = 40*/, qreal walkingSpeed/* = 1.0*/, int level/*=1*/)
	: QObject(0)
	, m_active(false)
    , m_maxHp(maxHp)
    , m_currentHp(maxHp)
    , m_walkingSpeed(walkingSpeed)
    , m_level(level)
	, m_rotationSprite(0.0)
	, m_pos(startWayPoint->pos())
	, m_destinationWayPoint(startWayPoint->nextWayPoint())
	, m_game(game)
	, m_sprite(sprite)
{
}

Enemy::~Enemy()
{
    m_attackedTowersList.clear();//被攻击列表清空
    m_destinationWayPoint = NULL;//路径点清空
    m_game = NULL;//不再指向主窗
}

void Enemy::doActivate()
{
	m_active = true;
}

void Enemy::move()
{
    if (!m_active)//没有激活的怪不能动
		return;

	if (collisionWithCircle(m_pos, 1, m_destinationWayPoint->pos(), 1))
	{
		// 敌人抵达了一个航点
		if (m_destinationWayPoint->nextWayPoint())
		{
			// 还有下一个航点
			m_pos = m_destinationWayPoint->pos();
			m_destinationWayPoint = m_destinationWayPoint->nextWayPoint();
		}
		else
		{
			// 表示进入基地
			m_game->getHpDamage();
			m_game->removedEnemy(this);
			return;
		}
	}

	// 还在前往航点的路上
	// 目标航点的坐标
	QPoint targetPoint = m_destinationWayPoint->pos();
	// 未来修改这个可以添加移动状态,加快,减慢,m_walkingSpeed是基准值

	// 向量标准化
	qreal movementSpeed = m_walkingSpeed;
	QVector2D normalized(targetPoint - m_pos);
	normalized.normalize();
	m_pos = m_pos + normalized.toPoint() * movementSpeed;

	// 确定敌人选择方向
	// 默认图片向左,需要修正180度转右
	m_rotationSprite = qRadiansToDegrees(qAtan2(normalized.y(), normalized.x())) + 180;
}

void Enemy::draw(QPainter *painter) const
{
	if (!m_active)
		return;

	painter->save();

	QPoint healthBarPoint = m_pos + QPoint(-Health_Bar_Width / 2 - 5, -ms_fixedSize.height() / 3);
	// 绘制血条
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::red);
	QRect healthBarBackRect(healthBarPoint, QSize(Health_Bar_Width, 2));
	painter->drawRect(healthBarBackRect);

    painter->setBrush(Qt::green);
	QRect healthBarRect(healthBarPoint, QSize((double)m_currentHp / m_maxHp * Health_Bar_Width, 2));
	painter->drawRect(healthBarRect);

	// 绘制偏转坐标,由中心+偏移=左上
	static const QPoint offsetPoint(-ms_fixedSize.width() / 2, -ms_fixedSize.height() / 2);
	painter->translate(m_pos);
	painter->rotate(m_rotationSprite);
	// 绘制敌人
	painter->drawPixmap(offsetPoint, m_sprite);

	painter->restore();
}

void Enemy::getRemoved()
{
	if (m_attackedTowersList.empty())
		return;

	foreach (Tower *attacker, m_attackedTowersList)
		attacker->targetKilled();
	// 通知game,此敌人已经阵亡
	m_game->removedEnemy(this);
}

void Enemy::getDamage(int damage)
{
	m_game->audioPlayer()->playSound(LaserShootSound);
	m_currentHp -= damage;

	// 阵亡,需要移除
	if (m_currentHp <= 0)
	{
		m_game->audioPlayer()->playSound(EnemyDestorySound);
		m_game->awardGold(200);
		getRemoved();
	}
}

void Enemy::getAttacked(Tower *attacker)
{
    m_attackedTowersList.push_back(attacker);
}

// 表明敌人已经逃离了攻击范围
void Enemy::gotLostSight(Tower *attacker)
{
	m_attackedTowersList.removeOne(attacker);
}

QPoint Enemy::pos() const
{
	return m_pos;
}

void Enemy::slowDown()
{
    if(!m_isSlowed){
        m_walkingSpeed = 1;
    }
}

//Enemy2-----------
Enemy2::Enemy2(WayPoint *startWayPoint, MainWindow *game,
             const QPixmap &sprite,
             int maxHp, qreal walkingSpeed, int level)
    :Enemy(startWayPoint,game,sprite,maxHp,walkingSpeed,level)
{
    m_speedUp = false;
}

Enemy2::~Enemy2()
{
    m_attackedTowersList.clear();
    m_destinationWayPoint = NULL;
    m_game = NULL;
}

void Enemy2::getDamage(int damage){
    Enemy::getDamage(damage);
    if(m_speedUp == false){
        m_speedUp = true;
        m_walkingSpeed = 3.0*m_walkingSpeed;
    }
}

//Enemy3-----------
Enemy3::Enemy3(WayPoint *startWayPoint, MainWindow *game,
             const QPixmap &sprite,
             int maxHp, qreal walkingSpeed, int level)
    :Enemy(startWayPoint,game,sprite,maxHp,walkingSpeed,level)
{
    m_recover = false;
}

Enemy3::~Enemy3()
{
    m_attackedTowersList.clear();
    m_destinationWayPoint = NULL;
    m_game = NULL;
}

void Enemy3::getDamage(int damage){
    if(m_recover == false){
        m_recover = true;
        Enemy::getDamage(int(damage/2));
    }
    else{
        m_recover = false;
        Enemy::getDamage(damage);
    }
}
