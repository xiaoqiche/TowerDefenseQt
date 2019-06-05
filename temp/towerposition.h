#define TOWERPOSITION_H

#include <QPoint>
#include <QSize>
#include <QPixmap>

class QPainter;

class TowerPosition
{
public:
    TowerPosition(QPoint pos, const QPixmap &sprite = QPixmap(":/image/open_spot.png"));

    void setHasTower(bool hasTower = true);//将此点置为：已安放塔
    bool hasTower() const;
    const QPoint centerPos() const;
    bool containPoint(const QPoint &pos) const;

    void draw(QPainter *painter) const;

private:
    bool		m_hasTower;
    QPoint		m_pos;
    QPixmap		m_sprite;

    static const QSize ms_fixedSize;
};

#endif // TOWERPOSITION_H
