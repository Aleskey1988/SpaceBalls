#include "GameField.h"

GameField::GameField(QWidget* parent)
    : QLabel(parent)
{
    ui.setupUi(this);

    setMouseTracking(true);

    qsrand(0);
    // allocate game field data
    balls.resize(fieldSize.width());
    for (int i = 0; i < fieldSize.width(); ++i)
        balls[i].resize(fieldSize.height());
    // fill game field rectangles and positions
    for (int i = 0; i < fieldSize.width(); ++i)
    {
        for (int j = 0; j < fieldSize.height(); ++j)
        {
            balls[i][j].SetType(Ball::Type(qrand() % Ball::GetBallNumber()));
            balls[i][j].SetRect(QRect(i * ballSize, j * ballSize,
                ballSize, ballSize));
            balls[i][j].SetPos(QPoint(i, j));
        }
    }
    shuffleBalls();

    ebp1.maxValue = 10000;
    ebp1.rect = QRect(0, 0, 100, 100);
    ebp1.pos = QPoint((extraBonusesArea.width() - ebp1.rect.width()) / 2, (extraBonusesArea.width() - ebp1.rect.width()) / 2);
    ebp2.maxValue = 50;
    ebp2.rect = QRect(0, 0, 100, 100);
    ebp2.pos = QPoint((extraBonusesArea.width() - ebp2.rect.width()) / 2, (extraBonusesArea.width() - ebp2.rect.width()) / 2 + ebp2.rect.height());
    ebp3.maxValue = 50000;
    ebp3.rect = QRect(0, 0, 100, 100);
    ebp3.pos = QPoint((extraBonusesArea.width() - ebp3.rect.width()) / 2, (extraBonusesArea.width() - ebp3.rect.width()) / 2 + ebp3.rect.height() * 2);

    // debug
    //balls[0][0].SetType(Ball::Ball6);
    //balls[0][1].SetType(Ball::Ball6);
    //balls[1][2].SetType(Ball::Ball6);
    //balls[0][3].SetType(Ball::Ball6);
    //balls[0][4].SetType(Ball::Ball6);

    background = QImage(":/misc/Resources/space.jpg").scaled(gameFieldSize);
    // convert SVG rextures to QImage
    textures << SvgToImage(QString(":/caps/Resources/caps/01-saturn.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/02-jupiter.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/03-uranus.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/04-neptune.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/05-mercury.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/06-earth.svg"));
    QImage emptyBall(ballSize, ballSize, QImage::Format_ARGB32);
    emptyBall.fill(Qt::transparent);
    textures << emptyBall;
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/4.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/5.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/6.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-1.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-3.svg"));
    extraBonus1Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-1-red.svg"));
    extraBonus1Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-1-violet.svg"));
    extraBonus1Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-1-yellow.svg"));
    extraBonus2Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2-left.svg"));
    extraBonus2Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2-right.svg"));
    extraBonus2Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2-top.svg"));
    extraBonus2Textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2-bottom.svg"));

    selectedImage = SvgToImage(QString(":/misc/Resources/select.svg"));
    // load sounds
    sounds << new QSound(":/sounds/Resources/sounds/remove-balls.wav");
    sounds << new QSound(":/sounds/Resources/sounds/wrong-move.wav");
    sounds << new QSound(":/sounds/Resources/sounds/use-bonus-4.wav");
    sounds << new QSound(":/sounds/Resources/sounds/use-bonus-5.wav");
    sounds << new QSound(":/sounds/Resources/sounds/use-bonus-6.wav");

    QMediaPlaylist* playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("Resources/playlist/DST-Aircord.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    playlist->shuffle();
    QMediaPlayer* music = new QMediaPlayer();
    music->setPlaylist(playlist);
    //music->play();

    connect(&timer, &QTimer::timeout, this, &GameField::updateGameField);
    timer.start(1000 / fps);
}

void GameField::Test()
{
    shuffleBalls();
}

void GameField::mousePressEvent(QMouseEvent* e)
{
    if (isUseMouse)
    {
        QPoint pos = e->pos();
        // game field area
        if (gameFieldArea.contains(pos))
        {
            pos -= gameFieldArea.topLeft();
            int x = pos.x() / ballSize;
            int y = pos.y() / ballSize;
            if (e->button() == Qt::LeftButton)
            {
                if (!isFirstSelected)
                {
                    isFirstSelected = true;
                    firstBall = &balls[x][y];
                    firstBall->SetSelected(true);
                }
                else
                {
                    swapBalls(x, y);
                }
            }
            // for debug purposes
            else if (e->button() == Qt::RightButton)
            {
                balls[x][y].SetType(Ball::Bonus6);
            }
        }
        // extra bonuses area
        else if (extraBonusesArea.contains(pos))
        {
            pos -= extraBonusesArea.topLeft();
            if (QRect(ebp1.pos, ebp1.rect.size()).contains(pos) &&
                ebp1.value == ebp1.maxValue)
            {
                QPoint pos = getRandomBallPos();
                balls[pos.x()][pos.y()].SetType(Ball::ExtraBonus1);
                ebp1.value = 0;
            }
            else if (QRect(ebp2.pos, ebp2.rect.size()).contains(pos) &&
                ebp2.value == ebp2.maxValue)
            {
                QPoint pos = getRandomBallPos();
                balls[pos.x()][pos.y()].SetType(Ball::ExtraBonus2);
                ebp2.value = 0;
            }
            else if (QRect(ebp3.pos, ebp3.rect.size()).contains(pos) &&
                ebp3.value == ebp3.maxValue)
            {
                QPoint pos = getRandomBallPos();
                balls[pos.x()][pos.y()].SetType(Ball::ExtraBonus3);
                ebp3.value = 0;
            }
        }
    }
}
void GameField::mouseMoveEvent(QMouseEvent* e)
{
    if (isUseMouse && e->buttons() != Qt::NoButton)
    {
        QPoint pos = e->pos();
        // game field area
        if (gameFieldArea.contains(pos))
        {
            pos -= gameFieldArea.topLeft();
            int x = pos.x() / ballSize;
            int y = pos.y() / ballSize;
            if (firstBall != nullptr)
            {
                QPoint p = firstBall->GetPos();
                if ((x - p.x() == -1 && y - p.y() == 0 && x >= 0) ||
                    (x - p.x() == 1 && y - p.y() == 0 && x <= fieldSize.width() - 1) ||
                    (y - p.y() == -1 && x - p.x() == 0 && y >= 0) ||
                    (y - p.y() == 1 && x - p.x() == 0 && y <= fieldSize.height() - 1))
                {
                    swapBalls(x, y);
                }
            }
        }
    }
}
void GameField::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (isUseMouse)
    {
        QPoint pos = e->pos();
        // game field area
        if (gameFieldArea.contains(pos))
        {
            pos -= gameFieldArea.topLeft();
            int x = pos.x() / ballSize;
            int y = pos.y() / ballSize;
            balls[x][y].SetSelected(false);
            if (balls[x][y].GetType() == Ball::Bonus4)
            {
                isUseMouse = false;
                QList<QPoint> shape;
                shape << QPoint(x, y);
                if (x > 0)
                    shape << QPoint(x - 1, y);
                if (x < fieldSize.width() - 1)
                    shape << QPoint(x + 1, y);
                if (y > 0)
                    shape << QPoint(x, y - 1);
                if (y < fieldSize.height() - 1)
                    shape << QPoint(x, y + 1);
                sounds[Sound::UseBonus4]->play();

                b4.rect = QRect(QPoint(x * ballSize + ballSize / 2,
                    y * ballSize + ballSize / 2),
                    QSize(0, 0));
                b4.isActive = true;
                connect(&removeTimer, &QTimer::timeout, this, [=]
                {
                    b4.rect.adjust(-1, -1, 1, 1);
                    removeCounter++;
                    if (removeCounter == (int)(ballSize * 1.3))
                    {
                        removeTimer.stop();
                        disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                        removeCounter = 0;

                        b4.isActive = false;
                        QList<QList<QPoint>> shapes;
                        shapes << shape;
                        removeBalls(shapes, RemoveType::Bonus);
                    }
                });
                removeTimer.start(timerTick);
            }
            else if (balls[x][y].GetType() == Ball::Bonus5)
            {
                isUseMouse = false;
                QList<QPoint> shape;
                for (int i = x - 1; i <= x + 1; i++)
                {
                    for (int j = y - 1; j <= y + 1; j++)
                    {
                        if (i >= 0 && i < fieldSize.width() && j >= 0 && j < fieldSize.height())
                            shape << QPoint(i, j);
                    }
                }
                for (int i = x - 1; i <= x + 1; i++)
                {
                    if (i >= 0 && i < fieldSize.width())
                    {
                        if (y - 2 >= 0)
                            shape << QPoint(i, y - 2);
                        if (y < fieldSize.height() - 2)
                            shape << QPoint(i, y + 2);
                    }
                }
                for (int j = y - 1; j <= y + 1; j++)
                {
                    if (j >= 0 && j < fieldSize.height())
                    {
                        if (x - 2 >= 0)
                            shape << QPoint(x - 2, j);
                        if (x < fieldSize.width() - 2)
                            shape << QPoint(x + 2, j);
                    }
                }
                sounds[Sound::UseBonus5]->play();

                bonus5Rect = QRect(QPoint(x * ballSize,
                    y * ballSize), QSize(ballSize, ballSize));
                isBonus5 = true;
                int step = 3;
                connect(&removeTimer, &QTimer::timeout, this, [=]
                {
                    bonus5Rect = bonus5Rect.adjusted(-step, -step, step, step);
                    removeCounter++;
                    if (removeCounter == ballSize / 2)
                    {
                        removeTimer.stop();
                        disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                        removeCounter = 0;

                        isBonus5 = false;
                        QList<QList<QPoint>> shapes;
                        shapes << shape;
                        removeBalls(shapes, RemoveType::Bonus);
                    }
                });
                removeTimer.start(timerTick);
            }
            else if (balls[x][y].GetType() == Ball::Bonus6)
            {
                isUseMouse = false;
                QList<QPoint> shape;
                for (int i = x - 2; i <= x + 2; i++)
                {
                    for (int j = y - 2; j <= y + 2; j++)
                    {
                        if (i >= 0 && i < fieldSize.width() && j >= 0 && j < fieldSize.height())
                            shape << QPoint(i, j);
                    }
                }
                for (int i = x - 2; i <= x + 2; i++)
                {
                    if (i >= 0 && i < fieldSize.width())
                    {
                        if (y - 3 >= 0)
                            shape << QPoint(i, y - 3);
                        if (y < fieldSize.height() - 3)
                            shape << QPoint(i, y + 3);
                    }
                }
                for (int j = y - 2; j <= y + 2; j++)
                {
                    if (j >= 0 && j < fieldSize.height())
                    {
                        if (x - 3 >= 0)
                            shape << QPoint(x - 3, j);
                        if (x < fieldSize.width() - 3)
                            shape << QPoint(x + 3, j);
                    }
                }
                sounds[Sound::UseBonus6]->play();

                b6.rect = QRect(QPoint(x * ballSize,
                    y * ballSize), QSize(ballSize, ballSize));
                b6.isStage1 = true;
                int step = 1;
                int numSteps = ballSize * 3;
                connect(&removeTimer, &QTimer::timeout, this, [=]
                {
                    b6.rect = b6.rect.adjusted(-step, -step, step, step);
                    b6.angle += step * 2;
                    removeCounter++;
                    if (removeCounter == numSteps)
                    {
                        removeTimer.stop();
                        disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                        removeCounter = 0;

                        b6.isStage1 = false;
                        QList<QList<QPoint>> shapes;
                        shapes << shape;
                        removeBalls(shapes, RemoveType::Bonus);
                    }
                });
                removeTimer.start(timerTick);
            }
            else if (balls[x][y].GetType() == Ball::ExtraBonus1)
            {
                isUseMouse = false;
                QList<QPoint> shape;
                // fill initial rects positions outside of game field
                for (int i = 0; i < eb1.numMeteors; i++)
                {
                    int side = qrand() % 4;
                    int x = 0;
                    int y = 0;
                    // left
                    if (side == 0)
                    {
                        x = -1;
                        y = qrand() % fieldSize.height();
                    }
                    // right
                    else if (side == 1)
                    {
                        x = fieldSize.width();
                        y = qrand() % fieldSize.height();
                    }
                    // top
                    else if (side == 2)
                    {
                        x = qrand() % fieldSize.width();
                        y = -1;
                    }
                    // bottom
                    else if (side == 3)
                    {
                        x = qrand() % fieldSize.width();
                        y = fieldSize.height();
                    }
                    eb1.startPoints << QPointF(x, y) * ballSize;
                }
                eb1.stage1CurPoints = eb1.startPoints;
                // fill rects destination positions on game field
                for (int i = 0; i < eb1.numMeteors; i++)
                {
                    QPoint pos = getRandomBallPos();
                    eb1.endPoints << QPointF(pos) * ballSize;
                    // fill shape points
                    shape << pos;
                    if (pos.x() > 0 && pos.y() > 0)
                        shape << QPoint(pos.x() - 1, pos.y() - 1);
                    if (pos.x() < fieldSize.width() - 1 && pos.y() > 0)
                        shape << QPoint(pos.x() + 1, pos.y() - 1);
                    if (pos.x() < fieldSize.width() - 1 && pos.y() < fieldSize.height() - 1)
                        shape << QPoint(pos.x() + 1, pos.y() + 1);
                    if (pos.x() > 0 && pos.y() < fieldSize.height() - 1)
                        shape << QPoint(pos.x() - 1, pos.y() + 1);
                }
                // remove shape points dublicates

                // 
                for (int i = 0; i < eb1.numMeteors; i++)
                {
                    eb1.meteorType << qrand() % 3;
                    eb1.angles << QLineF(eb1.startPoints[i], eb1.endPoints[i]).angle();
                }
                // start animation
                eb1.isStage1 = true;
                int numSteps = 250;
                int curStep = 0;
                connect(&removeTimer, &QTimer::timeout, this, [=] () mutable
                {
                    for (int i = 0; i < eb1.numMeteors; i++)
                    {
                        QPointF p1 = eb1.startPoints[i];
                        QPointF p2 = eb1.endPoints[i];
                        double x = p1.x() + ((double)p2.x() - p1.x()) * ((double)curStep / numSteps);
                        double y = p1.y() + ((double)p2.y() - p1.y()) * ((double)curStep / numSteps);
                        eb1.stage1CurPoints[i] = QPointF(x, y);
                    }
                    curStep++;
                    if (curStep == numSteps + 1)
                    {
                        removeTimer.stop();
                        disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                        removeCounter = 0;
                        eb1.isStage1 = false;
                        eb1.startPoints.clear();
                        eb1.angles.clear();
                        eb1.stage2CurPoints.resize(eb1.numMeteors);
                        for (int i = 0; i < eb1.endPoints.size(); i++)
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                eb1.stage2CurPoints[i] << eb1.endPoints[i];
                            }
                        }
                        eb1.opacity = 1.0;
                        // 
                        eb1.isStage2 = true;
                        double step = 1;
                        int numSteps = ballSize;
                        double opacityStep = step / numSteps;
                        connect(&removeTimer, &QTimer::timeout, this, [=]
                        {
                            for (int i = 0; i < eb1.endPoints.size(); i++)
                            {
                                eb1.stage2CurPoints[i][0] += QPointF(-step, -step);
                                eb1.stage2CurPoints[i][1] += QPointF(-step, step);
                                eb1.stage2CurPoints[i][2] += QPointF(step, step);
                                eb1.stage2CurPoints[i][3] += QPointF(step, -step);
                            }
                            eb1.opacity -= opacityStep / 2;
                            removeCounter++;
                            if (removeCounter == ballSize)
                            {
                                removeTimer.stop();
                                disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                                removeCounter = 0;
                                eb1.isStage2 = false;
                                eb1.endPoints.clear();
                                eb1.stage2CurPoints.clear();
                                eb1.meteorType.clear();

                                QList<QList<QPoint>> removeShapes;
                                removeShapes << shape;
                                removeBalls(removeShapes, RemoveType::Bonus);
                            }
                        });
                        removeTimer.start(timerTick);
                    }
                });
                removeTimer.start(timerTick);
            }
            else if (balls[x][y].GetType() == Ball::ExtraBonus2)
            {
                isUseMouse = false;
                QVector<QList<QPoint>> shapes(4);
                for (int i = x - 1; i > -1; i--)
                    shapes[0] << QPoint(i, y);
                for (int i = x + 1; i < fieldSize.width(); i++)
                    shapes[1] << QPoint(i, y);
                for (int j = y - 1; j > -1; j--)
                    shapes[2] << QPoint(x, j);
                for (int j = y + 1; j < fieldSize.height(); j++)
                    shapes[3] << QPoint(x, j);

                for (int i = 0; i < 4; i++)
                    extraBonus2Pos << QPoint(x * ballSize, y * ballSize);

                for (int i = 0; i < shapes.size(); i++)
                    extraBonus2Lines << QLine(
                        (x + 0.5) * ballSize,
                        (y + 0.5) * ballSize,
                        (x + 0.5) * ballSize,
                        (y + 0.5) * ballSize);

                isExtraBonus2Pos = true;
                int maxSize = qMax(shapes[0].size(), qMax(shapes[1].size(), qMax(shapes[2].size(), shapes[3].size())));
                int step = 2;
                isExtraBonus2Lines = true;

                connect(&removeTimer, &QTimer::timeout, this, [=]
                {
                    extraBonus2Transparent = (int)(192.0 * removeCounter / (double)(maxSize * ballSize / step));
                    extraBonus2Pen = QPen(QColor(255, 128, 0, extraBonus2Transparent), ballSize / 10);
                    if (extraBonus2Pos[0].x() > 0)
                    {
                        extraBonus2Pos[0] += QPoint(-step, 0);
                        extraBonus2Lines[0].setP2(QPoint(
                            extraBonus2Lines[0].p2().x() - step,
                            extraBonus2Lines[0].p2().y()));
                    }
                    if (extraBonus2Pos[1].x() < (fieldSize.width() - 1) * ballSize)
                    {
                        extraBonus2Pos[1] += QPoint(step, 0);
                        extraBonus2Lines[1].setP2(QPoint(
                            extraBonus2Lines[1].p2().x() + step,
                            extraBonus2Lines[1].p2().y()));
                    }
                    if (extraBonus2Pos[2].y() > 0)
                    {
                        extraBonus2Pos[2] += QPoint(0, -step);
                        extraBonus2Lines[2].setP2(QPoint(
                            extraBonus2Lines[2].p2().x(),
                            extraBonus2Lines[2].p2().y() - step));
                    }
                    if (extraBonus2Pos[3].y() < (fieldSize.height() - 1) * ballSize)
                    {
                        extraBonus2Pos[3] += QPoint(0, step);
                        extraBonus2Lines[3].setP2(QPoint(
                            extraBonus2Lines[3].p2().x(),
                            extraBonus2Lines[3].p2().y() + step));
                    }

                    removeCounter++;
                    if (removeCounter == maxSize * ballSize / step)
                    {
                        removeTimer.stop();
                        disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                        removeCounter = 0;

                        extraBonus2Pen.setCapStyle(Qt::RoundCap);
                        connect(&removeTimer, &QTimer::timeout, this, [=]
                        {
                            extraBonus2Transparent = (int)(192.0 + 63.0 * removeCounter / (double)(ballSize / 2));
                            extraBonus2Pen.setColor(QColor(255, 128, 0, extraBonus2Transparent));
                            extraBonus2Pen.setWidth(extraBonus2Pen.width() + 1);
                            removeCounter++;
                            if (removeCounter == ballSize / 2)
                            {
                                removeTimer.stop();
                                disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                                removeCounter = 0;

                                isExtraBonus2Pos = false;
                                extraBonus2Pos.clear();
                                isExtraBonus2Lines = false;
                                extraBonus2Lines.clear();

                                QList<QList<QPoint>> removeShapes;
                                for (int i = 0; i < shapes.size(); i++)
                                    removeShapes << shapes[i];
                                removeShapes << QList<QPoint>{QPoint(x, y)};
                                removeBalls(removeShapes, RemoveType::Bonus);
                            }
                        });
                        removeTimer.start(timerTick);
                    }
                });
                removeTimer.start(timerTick);
            }
            else if (balls[x][y].GetType() == Ball::ExtraBonus3)
            {
                isUseMouse = false;
                // get near balls types
                QSet<Ball::Type> typesSet;
                if (x > 0)
                    typesSet << balls[x - 1][y].GetType();
                if (x < fieldSize.width() - 1)
                    typesSet << balls[x + 1][y].GetType();
                if (y > 0)
                    typesSet << balls[x][y - 1].GetType();
                if (y < fieldSize.height() - 1)
                    typesSet << balls[x][y + 1].GetType();
                // get types numbers
                QList<Ball::Type> typesList = typesSet.toList();
                QVector<int> lenghts(typesList.size());
                for (int k = 0; k < typesList.size(); ++k)
                {
                    for (int i = 0; i < fieldSize.width(); ++i)
                    {
                        for (int j = 0; j < fieldSize.height(); ++j)
                        {
                            if (balls[i][j].GetType() == typesList[k])
                                lenghts[k]++;
                        }
                    }
                }
                // get type with maximum number
                QVector<QPair<Ball::Type, int>> pairs;
                for (int i = 0; i < typesList.size(); i++)
                {
                    pairs << QPair<Ball::Type, int>(typesList[i], lenghts[i]);
                }
                std::sort(pairs.begin(), pairs.end(), [=]
                (const QPair<Ball::Type, int>& left, const QPair<Ball::Type, int>& right)
                {
                    return left.second > right.second;
                });
                QPair<Ball::Type, int> max = pairs[0];
                // get points of type with maximum number
                QList<QPoint> points;
                points << QPoint(x, y);
                for (int i = 0; i < fieldSize.width(); ++i)
                {
                    for (int j = 0; j < fieldSize.height(); ++j)
                    {
                        if (balls[i][j].GetType() == max.first)
                            points << QPoint(i, j);
                    }
                }
                // connect points to get closed contour without self-crosses
                QPointF center;
                for (int i = 0; i < points.size(); i++)
                    center += points[i];
                center /= points.size();
                struct LineData
                {
                    QPoint p;
                    double angle;
                    double length;
                };
                QList<LineData> lineData;
                for (int i = 0; i < points.size(); i++)
                {
                    LineData data;
                    data.p = points[i];
                    data.angle = QLineF(center, points[i]).angle();
                    data.length = QLineF(center, points[i]).length();
                    lineData << data;
                }
                std::sort(lineData.begin(), lineData.end(), [=] (LineData& left, LineData& right)
                {
                    if (qFuzzyCompare(left.angle, right.angle))
                        return left.length < right.length;
                    return left.angle < right.angle;
                });
                points.clear();
                for (int i = 0; i < lineData.size(); i++)
                    points << lineData[i].p;
                // 
                int initialPos = points.indexOf(QPoint(x, y));
                std::rotate(points.begin(), points.begin() + initialPos, points.end());
                // initial polygon
                QPolygonF polygon;
                for (int i = 0; i < points.size(); i++)
                    polygon << QPoint(points[i].x() * ballSize + ballSize / 2,
                        points[i].y() * ballSize + ballSize / 2);
                QPainterPath path;
                path.addPolygon(polygon);
                path.closeSubpath();
                path.setFillRule(Qt::FillRule::WindingFill);
                eb3.path = path;

                eb3.transparent = 0;
                eb3.pen = QPen(Qt::blue, ballSize / 10, Qt::SolidLine, Qt::RoundCap);
                for (int i = 0; i < points.size(); i++)
                {
                    eb3.lines << QLine(points[i].x() * ballSize + ballSize / 2,
                        points[i].y() * ballSize + ballSize / 2,
                        points[i].x() * ballSize + ballSize / 2,
                        points[i].y() * ballSize + ballSize / 2);
                }
                int lenght = 10;
                int lenghtCounter = 1;
                int currentPoint = 0;
                int linePos = 0;
                // start animation
                eb3.isStage1 = true;
                connect(&removeTimer, &QTimer::timeout, this, [=] () mutable
                {
                    QPoint p1 = points[currentPoint] * ballSize + QPoint(ballSize / 2, ballSize / 2);
                    QPoint p2 = points[currentPoint == points.size() - 1 ? 0 : currentPoint + 1] * ballSize + QPoint(ballSize / 2, ballSize / 2);
                    double x = ((double)p2.x() - p1.x()) / QLineF(p1, p2).length() * lenght * lenghtCounter;
                    double y = ((double)p2.y() - p1.y()) / QLineF(p1, p2).length() * lenght * lenghtCounter;
                    QPoint p = p1 + QPoint(qRound(x), qRound(y));
                    eb3.lines[currentPoint].setP2(p);
                    lenghtCounter++;
                    if (QLineF(eb3.lines[currentPoint]).length() > QLineF(p1, p2).length())
                    {
                        currentPoint++;
                        lenghtCounter = 1;
                    }
                    if (currentPoint == points.size())
                    {
                        removeTimer.stop();
                        disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                        removeCounter = 0;
                        eb3.isStage1 = false;
                        eb3.isStage2 = true;
                        connect(&removeTimer, &QTimer::timeout, this, [=] () mutable
                        {
                            eb3.transparent += 3;
                            eb3.brush = QBrush(QColor(64, 64, 255, eb3.transparent));
                            removeCounter++;
                            if (removeCounter == 83)
                            {
                                removeTimer.stop();
                                disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                                removeCounter = 0;
                                eb3.isStage2 = false;
                                eb3.lines.clear();
                                QList<QList<QPoint>> removeShapes;
                                // add inner points
                                for (int i = 0; i < fieldSize.width(); ++i)
                                {
                                    for (int j = 0; j < fieldSize.height(); ++j)
                                    {
                                        if (polygon.containsPoint(QPoint(i * ballSize + ballSize / 2,
                                            j * ballSize + ballSize / 2), Qt::WindingFill))
                                            points << QPoint(i, j);
                                    }
                                }
                                removeShapes << points;
                                removeBalls(removeShapes, RemoveType::Bonus);
                            }
                        });
                        removeTimer.start(timerTick);
                    }
                });
                removeTimer.start(timerTick);
            }
        }
    }
}

void GameField::updateGameField()
{
    QPixmap pixmap(gameFieldSize);
    QPainter p(&pixmap);
    p.drawImage(QPoint(0, 0), background);

    // bonus 6
    p.translate(gameFieldArea.topLeft());
    if (b6.isStage1)
    {
        QSvgRenderer r(QString(":/bonuses/Resources/bonuses/6.svg"));
        QImage image(b6.rect.width(), b6.rect.width(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        r.render(&painter, QRectF(0, 0, b6.rect.width(), b6.rect.height()));
        QTransform matrix;
        matrix.rotate(b6.angle);
        // TODO: crash if use image, not imageCopy
        auto imageCopy = image.transformed(matrix, Qt::SmoothTransformation);
        // keep rotation center
        p.drawImage(b6.rect.topLeft() - QPoint(imageCopy.rect().width() - b6.rect.width(), imageCopy.rect().height() - b6.rect.height()) / 2, imageCopy);
    }

    // game field
    p.translate(-gameFieldArea.topLeft());
    p.setPen(framePen);
    p.setBrush(frameBrush);
    p.drawRect(gameFieldArea);
    p.translate(gameFieldArea.topLeft());

    // balls and selection
    for (int i = 0; i < fieldSize.width(); ++i)
    {
        for (int j = 0; j < fieldSize.height(); ++j)
        {
            // common ball
            p.drawImage(balls[i][j].GetRect().topLeft(), textures[balls[i][j].GetType()].scaled(balls[i][j].GetRect().size()));
            // selection
            if (balls[i][j].GetSelected())
                p.drawImage(balls[i][j].GetRect().topLeft(), selectedImage.scaled(balls[i][j].GetRect().size()));
        }
    }

    // bonus 4
    if (b4.isActive)
    {
        p.setPen(b4.pen);
        p.setBrush(b4.brush);
        p.drawEllipse(b4.rect);
    }

    // bonus 5
    if (isBonus5)
    {
        QSvgRenderer r(QString(":/bonuses/Resources/bonuses/5.svg"));
        QImage image(bonus5Rect.width() - ballGap * 2, bonus5Rect.height() - ballGap * 2, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        r.render(&painter, QRectF(0, 0, bonus5Rect.width() - ballGap * 2, bonus5Rect.height() - ballGap * 2));
        p.drawImage(bonus5Rect.topLeft() + QPoint(ballGap, ballGap), image);
    }

    // extra bonus 1
    if (eb1.isStage1)
    {
        for (int i = 0; i < eb1.stage1CurPoints.size(); i++)
        {
            QImage image = extraBonus1Textures[eb1.meteorType[i]];
            QTransform matrix;
            matrix.rotate(360.0 - eb1.angles[i]);
            image = image.transformed(matrix, Qt::SmoothTransformation);
            p.drawImage(eb1.stage1CurPoints[i] - QPoint(image.width() * ballGapPercent, image.height() * ballGapPercent), image);
        }
    }
    if (eb1.isStage2)
    {
        QList<double> angles {135, 225, 315, 45};
        for (int i = 0; i < eb1.stage1CurPoints.size(); i++)
        {
            for (int j = 0; j < eb1.stage2CurPoints[i].size(); j++)
            {
                QImage image = extraBonus1Textures[eb1.meteorType[i]];
                QTransform matrix;
                matrix.rotate(360.0 - angles[j]);
                image = image.transformed(matrix);
                p.setOpacity(eb1.opacity);
                p.drawImage(eb1.stage2CurPoints[i][j] - QPoint(image.width() * ballGapPercent, image.height() * ballGapPercent), image);
            }
        }
    }

    // extra bonus 2
    if (isExtraBonus2Pos)
    {
        for (int i = 0; i < extraBonus2Textures.size(); i++)
            p.drawImage(extraBonus2Pos[i], extraBonus2Textures[i]);
    }
    if (isExtraBonus2Lines)
    {
        p.setPen(extraBonus2Pen);
        for (int i = 0; i < extraBonus2Lines.size(); i++)
            p.drawLine(extraBonus2Lines[i]);
    }

    // extra bonus 3
    if (eb3.isStage1)
    {
        p.setPen(eb3.pen);
        for (int i = 0; i < eb3.lines.size(); i++)
            p.drawLine(eb3.lines[i]);
    }
    if (eb3.isStage2)
    {
        p.setPen(eb3.pen);
        p.setBrush(eb3.brush);
        for (int i = 0; i < eb3.lines.size(); i++)
            p.drawLine(eb3.lines[i]);
        p.drawPath(eb3.path);
    }

    // score
    p.translate(-gameFieldArea.topLeft());
    p.setPen(framePen);
    p.setBrush(frameBrush);
    p.drawRect(scoreArea);
    p.translate(scoreArea.topLeft());
    p.setFont(QFont("Arial", 30));
    p.setPen(QPen(Qt::blue));
    p.drawText(15, 60, QString::number(score));
    
    // extra bonuses
    p.translate(-scoreArea.topLeft());
    p.setPen(framePen);
    p.setBrush(frameBrush);
    p.drawRect(extraBonusesArea);
    p.translate(extraBonusesArea.topLeft());

    auto drawExtraBonus = [&] (ExtraBonusProgress& ebp, QString& imageName, QPainter& painter)
    {
        QPixmap pixmap(ebp.rect.size());
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        double progress = (double)ebp.value / ebp.maxValue * 360;
        p.setPen(QPen(Qt::transparent));
        p.setBrush(QBrush(QColor(192, 192, 255)));
        // bonus is not filled; draw pie
        if (progress < 360.0)
        {
            path.moveTo(pixmap.rect().adjusted(adjust, adjust, -adjust, -adjust).center());
            path.arcTo(ebp.rect.adjusted(adjust, adjust, -adjust, -adjust), -90, -progress);
            path.closeSubpath();
            p.drawPath(path);
        }
        // bonus is filled; draw ellipse
        else
        {
            p.drawEllipse(ebp.rect.adjusted(adjust, adjust, -adjust, -adjust));
        }
        // draw outer bonus image circle
        p.setPen(QPen(QColor(64, 64, 255), 2));
        p.setBrush(QBrush(Qt::transparent));
        p.drawEllipse(ebp.rect.adjusted(adjust, adjust, -adjust, -adjust));
        // draw bonus image background
        p.setBrush(QBrush(QColor(128, 128, 255)));
        p.drawEllipse(ebp.rect.adjusted(adjust * 3, adjust * 3, -adjust * 3, -adjust * 3));
        // draw bonus image
        QSvgRenderer r(imageName);
        QImage image(50, 50, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        r.render(&p, QRectF(25, 25, 50, 50));
        painter.drawPixmap(ebp.pos, pixmap);
    };

    drawExtraBonus(ebp1, QString(":/bonuses/Resources/bonuses/extra-1.svg"), p);
    drawExtraBonus(ebp2, QString(":/bonuses/Resources/bonuses/extra-2.svg"), p);
    drawExtraBonus(ebp3, QString(":/bonuses/Resources/bonuses/extra-3.svg"), p);

    
    setPixmap(pixmap);
}
QList<QPoint> GameField::getShape(int x, int y)
{
    QList<QPoint> points;
    QStack<QPoint> stack;
    stack << QPoint(x, y);
    Ball::Type type = chainBalls[x][y].type;
    while (!stack.isEmpty())
    {
        QPoint p = stack.pop();
        points << p;
        chainBalls[p.x()][p.y()].checked = true;
        if (p.x() + 1 < fieldSize.width() && chainBalls[p.x() + 1][p.y()].type == type && !chainBalls[p.x() + 1][p.y()].checked)
        {
            stack << QPoint(p.x() + 1, p.y());
            chainBalls[p.x() + 1][p.y()].checked = true;
        }
        if (p.x() > 0 && chainBalls[p.x() - 1][p.y()].type == type && !chainBalls[p.x() - 1][p.y()].checked)
        {
            stack << QPoint(p.x() - 1, p.y());
            chainBalls[p.x() - 1][p.y()].checked = true;
        }
        if (p.y() + 1 < fieldSize.height() && chainBalls[p.x()][p.y() + 1].type == type && !chainBalls[p.x()][p.y() + 1].checked)
        {
            stack << QPoint(p.x(), p.y() + 1);
            chainBalls[p.x()][p.y() + 1].checked = true;
        }
        if (p.y() > 0 && chainBalls[p.x()][p.y() - 1].type == type && !chainBalls[p.x()][p.y() - 1].checked)
        {
            stack << QPoint(p.x(), p.y() - 1);
            chainBalls[p.x()][p.y() - 1].checked = true;
        }
    }
    return points;
}
QList<QList<QPoint>> GameField::getShapes(QVector<QVector<Ball>>& balls)
{
    chainBalls.clear();
    chainBalls.resize(fieldSize.width());
    for (int i = 0; i < fieldSize.width(); ++i)
        chainBalls[i].resize(fieldSize.height());
    // fill field mask
    for (int i = 0; i < fieldSize.width(); ++i)
    {
        for (int j = 0; j < fieldSize.height(); ++j)
        {
            chainBalls[i][j].type = balls[i][j].GetType();
        }
    }
    // get shapes
    QList<QList<QPoint>> shapes;
    for (int i = 0; i < fieldSize.width(); i++)
    {
        for (int j = 0; j < fieldSize.height(); j++)
        {
            if (!chainBalls[i][j].checked)
            {
                QList<QPoint> shape = getShape(i, j);
                if (shape.size() > 2)
                    shapes << shape;
            }
        }
    }
    return shapes;
}
QList<QList<QPoint>> GameField::getLineShapes(QList<QList<QPoint>>& shapes)
{
    QVector<QVector<int>> shapesField;
    shapesField.resize(fieldSize.width());
    for (int i = 0; i < fieldSize.width(); ++i)
        shapesField[i].resize(fieldSize.height());

    for (int i = 0; i < fieldSize.width(); i++)
    {
        for (int j = 0; j < fieldSize.height(); j++)
        {
            shapesField[i][j] = -1;
        }
    }

    QList<QList<QPoint>> lineShapes;
    for (int i = 0; i < shapes.size(); i++)
    {
        for (int j = 0; j < shapes[i].size(); j++)
        {
            QPoint p = shapes[i][j];
            shapesField[p.x()][p.y()] = i;
        }
    }

    for (int x = 0; x < shapes.size(); x++)
    {
        QList<QPoint> points;
        for (int y = 0; y < shapes[x].size(); y++)
        {
            QPoint p = shapes[x][y];
            int value = shapesField[p.x()][p.y()];
            QList<QPoint> leftPoints;
            QList<QPoint> rightPoints;
            QList<QPoint> topPoints;
            QList<QPoint> bottomPoints;
            int i = p.x();
            while (i >= 0 && shapesField[i][p.y()] == value)
            {
                leftPoints << QPoint(i, p.y());
                i--;
            }
            i = p.x();
            while (i < fieldSize.width() && shapesField[i][p.y()] == value)
            {
                rightPoints << QPoint(i, p.y());
                i++;
            }
            int j = p.y();
            while (j >= 0 && shapesField[p.x()][j] == value)
            {
                topPoints << QPoint(p.x(), j);
                j--;
            }
            j = p.y();
            while (j < fieldSize.height() && shapesField[p.x()][j] == value)
            {
                bottomPoints << QPoint(p.x(), j);
                j++;
            }
            if (leftPoints.size() > 2)
                points << leftPoints;
            if (rightPoints.size() > 2)
                points << rightPoints;
            if (topPoints.size() > 2)
                points << topPoints;
            if (bottomPoints.size() > 2)
                points << bottomPoints;
        }
        QList<QPoint> uniquePoints;
        if (points.size() > 3)
        {
            for (const auto& point : points)
            {
                if (!uniquePoints.contains(point))
                    uniquePoints << point;
            }
        }
        if (!uniquePoints.isEmpty())
            lineShapes << uniquePoints;
    }

    return lineShapes;
}
void GameField::removeBalls(QList<QList<QPoint>>& shapes, RemoveType removeType)
{
    if (!shapes.isEmpty())
    {
        sounds[Sound::RemoveBalls]->play();
        // update score
        prevScore = score;
        for (int i = 0; i < shapes.size(); i++)
        {
            score += (shapes[i].size() - 2) * 100;
        }
        // update extra bonus 1 stats
        int delta = score - prevScore;
        ebp1.value += delta;
        if (ebp1.value >= ebp1.maxValue)
        {
            ebp1.value = ebp1.maxValue;
        }
        // update bonus 3 stats
        if (removeType == RemoveType::Bonus)
        {
            ebp3.value += delta;
            if (ebp3.value >= ebp3.maxValue)
            {
                ebp3.value = ebp3.maxValue;
            }
        }
        // add bonuses
        if (removeType == RemoveType::Ball)
        {
            QVector<int> bonuses(3);
            // fill bonuses data
            for (int i = 0; i < shapes.size(); i++)
            {
                if (shapes[i].size() == 4)
                    bonuses[0]++;
                else if (shapes[i].size() == 5)
                    bonuses[1]++;
                else if (shapes[i].size() > 5)
                    bonuses[2]++;
            }
            // update extra bonus 2 stats
            ebp2.value += bonuses[0];
            ebp2.value += bonuses[1] * 2;
            ebp2.value += bonuses[2] * 3;
            if (ebp2.value >= ebp2.maxValue)
            {
                ebp2.value = ebp2.maxValue;
            }
            for (int i = 0; i < bonuses.size(); i++)
            {
                for (int j = 0; j < bonuses[i]; j++)
                {
                    int x = 0;
                    int y = 0;
                    do
                    {
                        int x = qrand() % fieldSize.width();
                        int y = qrand() % fieldSize.height();
                        if (balls[x][y].IsBall())
                        {
                            balls[x][y].SetType(Ball::Type(Ball::Bonus4 + i));
                            break;
                        }
                    }
                    while (true);
                }
            }
        }
        connect(&removeTimer, &QTimer::timeout, this, [=]
        {
            for (int i = 0; i < shapes.size(); i++)
            {
                for (int j = 0; j < shapes[i].size(); j++)
                {
                    QPoint p = shapes[i][j];
                    balls[p.x()][p.y()].SetRect(balls[p.x()][p.y()].GetRect().adjusted(1, 1, -1, -1));
                }
            }
            removeCounter++;
            if (removeCounter == ballSize / 2)
            {
                removeCounter = 0;
                removeTimer.stop();
                disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                for (int i = 0; i < shapes.size(); i++)
                {
                    for (int j = 0; j < shapes[i].size(); j++)
                    {
                        QPoint p = shapes[i][j];
                        balls[p.x()][p.y()].SetType(Ball::Empty);
                    }
                }
                // drop balls
                QList<QPair<QPoint, QPoint>> dropData = getDropData();
                connect(&dropTimer, &QTimer::timeout, this, [=]
                {
                    for (int i = 0; i < dropData.size(); i++)
                    {
                        QPoint p = dropData[i].first;
                        Ball* ball = &balls[p.x()][p.y()];
                        int delta = dropData[i].second.y() - dropData[i].first.y();
                        ball->SetRect(QRect(ball->GetRect().adjusted(0, delta, 0, delta)));
                    }
                    dropCounter++;
                    if (dropData.isEmpty())
                        dropCounter = ballSize;
                    if (dropCounter == ballSize)
                    {
                        dropCounter = 0;
                        dropTimer.stop();
                        disconnect(&dropTimer, &QTimer::timeout, this, nullptr);
                        // drop balls and move empty places to the top
                        for (int i = 0; i < balls.size(); i++)
                        {
                            QVector<Ball::Type> types;
                            for (int j = 0; j < fieldSize.height(); j++)
                            {
                                if (balls[i][j].GetType() != Ball::Empty)
                                    types << balls[i][j].GetType();
                            }
                            int emptyPlacesNum = fieldSize.height() - types.size();
                            for (int j = 0; j < emptyPlacesNum; j++)
                                types.push_front(Ball::Empty);

                            for (int j = 0; j < fieldSize.height(); j++)
                            {
                                Ball* ball = &balls[i][j];
                                ball->SetType(types[j]);
                                if (ball->GetType() == Ball::Empty)
                                {
                                    // ballSize % 2 is initial rect geometry
                                    // for odd ballSize it is 1, otherwise 0
                                    ball->SetRect(QRect(ball->GetPos().x() * ballSize + ballSize / 2,
                                        ball->GetPos().y() * ballSize + ballSize / 2, ballSize % 2, ballSize % 2));
                                }
                                else
                                {
                                    ball->SetRect(QRect(ball->GetPos().x() * ballSize, ball->GetPos().y() * ballSize,
                                        ballSize, ballSize));
                                }
                            }
                        }
                        // fill empty places
                        QList<QPoint> emptyPlaces;
                        for (int i = 0; i < fieldSize.width(); i++)
                        {
                            for (int j = 0; j < fieldSize.height(); j++)
                            {
                                if (balls[i][j].GetType() == Ball::Empty)
                                {
                                    emptyPlaces << QPoint(i, j);
                                    balls[i][j].SetType(Ball::Type(qrand() % Ball::GetBallNumber()));
                                }
                            }
                        }
                        connect(&fillTimer, &QTimer::timeout, this, [=]
                        {
                            for (int i = 0; i < emptyPlaces.size(); i++)
                            {
                                QPoint p = emptyPlaces[i];
                                balls[p.x()][p.y()].SetRect(balls[p.x()][p.y()].GetRect().adjusted(-1, -1, 1, 1));
                            }
                            ++fillCounter;
                            if (fillCounter == ballSize / 2)
                            {
                                fillCounter = 0;
                                fillTimer.stop();
                                disconnect(&fillTimer, &QTimer::timeout, this, nullptr);
                                removeBalls(getLineShapes(getShapes(balls)), RemoveType::Ball);
                            }
                        });
                        fillTimer.start(timerTick);
                    }
                });
                dropTimer.start(timerTick);
            }
        });
        removeTimer.start(timerTick);
    }
    else
    {
        QList<PossibleMove> moves = getPossibleMoves();
        // shuffle balls is there are no possible moves
        if (moves.isEmpty())
        {
            shuffleBalls();
        }
        isUseMouse = true;
    }
}
QList<QPair<QPoint, QPoint>> GameField::getDropData()
{
    QList<QPair<QPoint, QPoint>> dropData;
    for (int i = 0; i < fieldSize.width(); i++)
    {
        for (int j = 0; j < fieldSize.height(); j++)
        {
            if (balls[i][j].GetType() != Ball::Empty)
            {
                int emptyCounter = 0;
                for (int y = j; y < fieldSize.height(); ++y)
                {
                    if (balls[i][y].GetType() == Ball::Empty)
                        emptyCounter++;
                }
                if (emptyCounter > 0)
                {
                    QPair<QPoint, QPoint> pair;
                    pair.first = QPoint(i, j);
                    pair.second = QPoint(i, j + emptyCounter);
                    dropData << pair;
                }
            }
        }
    }
    return dropData;
}
QImage GameField::SvgToImage(QString& fileName)
{
    QSvgRenderer r(fileName);
    QImage image(ballSize, ballSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter p(&image);
    r.render(&p, QRectF(ballGap, ballGap, ballSize - ballGap * 2, ballSize - ballGap * 2));
    return image;
}
QList<GameField::PossibleMove> GameField::getPossibleMoves()
{
    QList<PossibleMove> moves;
    // horizontal swaps
    for (int i = 0; i < fieldSize.width() - 1; i++)
    {
        for (int j = 0; j < fieldSize.height(); j++)
        {
            // create data copy
            QVector<QVector<Ball>> ballsCopy = balls;
            // emulate balls exchange
            Ball::Type tmp = ballsCopy[i][j].GetType();
            ballsCopy[i][j].SetType(ballsCopy[i + 1][j].GetType());
            ballsCopy[i + 1][j].SetType(tmp);
            // get shapes for modificated copy
            QList<QList<QPoint>> shapes = getLineShapes(getShapes(ballsCopy));
            if (!shapes.isEmpty())
            {
                // sort to set the largest shape to the top
                qSort(shapes.begin(), shapes.end(), [=]
                (QList<QPoint>& shapes1, QList<QPoint>& shapes2)
                {
                    return shapes1.size() > shapes2.size();
                });
                QList<QPoint> maxShape = shapes.first();
                PossibleMove move(MoveType::Ball, QPoint(i, j), QPoint(i + 1, j), maxShape.size());
                moves << move;
            }
        }
    }
    // vertical swaps
    for (int i = 0; i < fieldSize.width(); i++)
    {
        for (int j = 0; j < fieldSize.height() - 1; j++)
        {
            // create data copy
            QVector<QVector<Ball>> ballsCopy = balls;
            // emulate balls exchange
            Ball::Type tmp = ballsCopy[i][j].GetType();
            ballsCopy[i][j].SetType(ballsCopy[i][j + 1].GetType());
            ballsCopy[i][j + 1].SetType(tmp);
            // get shapes for modificated copy
            QList<QList<QPoint>> shapes = getLineShapes(getShapes(ballsCopy));
            if (!shapes.isEmpty())
            {
                // sort to set the largest shape to the top
                qSort(shapes.begin(), shapes.end(), [=]
                (QList<QPoint>& shapes1, QList<QPoint>& shapes2)
                {
                    return shapes1.size() > shapes2.size();
                });
                QList<QPoint> maxShape = shapes.first();
                PossibleMove move(MoveType::Ball, QPoint(i, j), QPoint(i, j + 1), maxShape.size());
                moves << move;
            }
        }
    }
    // sort to set the best move to the top of moves list
    qSort(moves.begin(), moves.end(), [=]
    (PossibleMove& m1, PossibleMove& m2)
    {
        return m1.length > m2.length;
    });
    return moves;
}
void GameField::shuffleBalls()
{
    QList<QPoint> points;
    // fill points
    for (int i = 0; i < fieldSize.width(); i++)
    {
        for (int j = 0; j < fieldSize.height(); j++)
        {
            points << QPoint(i, j);
        }
    }
    do
    {
        // shuffle points
        for (int i = 0; i < std::pow(fieldSize.width() * fieldSize.height(), 2); i++)
        {
            int x = qrand() % points.size();
            int y = qrand() % points.size();
            qSwap(points[x], points[y]);
        }
        // apply shuffled points for game field
        QVector<QVector<Ball::Type>> types;
        types.resize(fieldSize.width());
        for (int i = 0; i < fieldSize.width(); ++i)
            types[i].resize(fieldSize.height());
        for (int i = 0; i < fieldSize.width(); i++)
        {
            for (int j = 0; j < fieldSize.height(); j++)
            {
                types[i][j] = balls[i][j].GetType();
            }
        }
        int counter = 0;
        for (int i = 0; i < fieldSize.width(); i++)
        {
            for (int j = 0; j < fieldSize.height(); j++)
            {
                QPoint p = points[counter];
                balls[i][j].SetType(types[p.x()][p.y()]);
                counter++;
            }
        }
    }
    while (!getLineShapes(getShapes(balls)).isEmpty() || getPossibleMoves().isEmpty());
}
QPoint GameField::getRandomBallPos()
{
    int x = 0;
    int y = 0;
    do
    {
        x = qrand() % fieldSize.width();
        y = qrand() % fieldSize.height();
    }
    while (!balls[x][y].IsBall());
    // TODO: check if there are no balls
    return QPoint(x, y);
}
void GameField::swapBalls(int x, int y)
{
    firstBall->SetSelected(false);
    secondBall = &balls[x][y];
    QPoint delta = firstBall->GetPos() - secondBall->GetPos();
    if (qAbs(delta.x()) + qAbs(delta.y()) == 1)
    {
        isUseMouse = false;
        connect(&ballsSwapTimer, &QTimer::timeout, this, [=]
        {
            firstBall->SetRect(firstBall->GetRect().adjusted(
                -delta.x(), -delta.y(), -delta.x(), -delta.y()));
            secondBall->SetRect(secondBall->GetRect().adjusted(
                delta.x(), delta.y(), delta.x(), delta.y()));
            ++swapCounter;
            if (swapCounter == ballSize)
            {
                swapCounter = 0;
                ballsSwapTimer.stop();
                disconnect(&ballsSwapTimer, &QTimer::timeout, this, nullptr);
                firstBall->SetRect(QRect(firstBall->GetPos().x() * ballSize, firstBall->GetPos().y() * ballSize,
                    ballSize, ballSize));
                secondBall->SetRect(QRect(secondBall->GetPos().x() * ballSize, secondBall->GetPos().y() * ballSize,
                    ballSize, ballSize));
                Ball::Type tmp = firstBall->GetType();
                firstBall->SetType(secondBall->GetType());
                secondBall->SetType(tmp);
                // 
                QList<QList<QPoint>> shapes = getLineShapes(getShapes(balls));
                if (!shapes.empty())
                    removeBalls(shapes, RemoveType::Ball);
                else
                {
                    sounds[Sound::WrongMove]->play();
                    QPoint delta = firstBall->GetPos() - secondBall->GetPos();
                    if (qAbs(delta.x()) + qAbs(delta.y()) == 1)
                    {
                        connect(&ballsSwapTimer, &QTimer::timeout, this, [=]
                        {
                            firstBall->SetRect(firstBall->GetRect().adjusted(
                                -delta.x(), -delta.y(), -delta.x(), -delta.y()));
                            secondBall->SetRect(secondBall->GetRect().adjusted(
                                delta.x(), delta.y(), delta.x(), delta.y()));
                            ++swapCounter;
                            if (swapCounter == ballSize)
                            {
                                swapCounter = 0;
                                ballsSwapTimer.stop();
                                disconnect(&ballsSwapTimer, &QTimer::timeout, this, nullptr);
                                firstBall->SetRect(QRect(firstBall->GetPos().x() * ballSize, firstBall->GetPos().y() * ballSize,
                                    ballSize, ballSize));
                                secondBall->SetRect(QRect(secondBall->GetPos().x() * ballSize, secondBall->GetPos().y() * ballSize,
                                    ballSize, ballSize));
                                Ball::Type tmp = firstBall->GetType();
                                firstBall->SetType(secondBall->GetType());
                                secondBall->SetType(tmp);
                                isUseMouse = true;
                            }
                        });
                        ballsSwapTimer.start(timerTick / 2);
                    }
                }
            }
        });
        ballsSwapTimer.start(timerTick / 2);
        isFirstSelected = false;
    }
    else
    {
        isFirstSelected = true;
        firstBall = &balls[x][y];
        firstBall->SetSelected(true);
    }
}