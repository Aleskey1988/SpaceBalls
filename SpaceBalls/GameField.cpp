#include "GameField.h"

GameField::GameField(QWidget* parent)
    : QLabel(parent)
{
    ui.setupUi(this);

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
            balls[i][j].SetRect(QRect(i * ballSize.width() + ballGap, j * ballSize.height() + ballGap,
                ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
            balls[i][j].SetPos(QPoint(i, j));
        }
    }
    shuffleCaps();

    // debug
    //balls[0][0].SetType(Ball::Cap6);
    //balls[0][1].SetType(Ball::Cap6);
    //balls[1][2].SetType(Ball::Cap6);
    //balls[0][3].SetType(Ball::Cap6);
    //balls[0][4].SetType(Ball::Cap6);

    background = QImage(":/misc/Resources/space.jpg").scaled(
        fieldSize.width() * ballSize.width(), fieldSize.height() * ballSize.height());
    // convert SVG rextures to QImage
    textures << SvgToImage(QString(":/caps/Resources/caps/01-saturn.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/02-jupiter.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/03-uranus.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/04-neptune.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/05-mercury.svg"));
    textures << SvgToImage(QString(":/caps/Resources/caps/06-earth.svg"));
    QImage emptyBall(ballSize.width(), ballSize.height(), QImage::Format_ARGB32);
    emptyBall.fill(Qt::transparent);
    textures << emptyBall;
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/4.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/5.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/6.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-1.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-3.svg"));
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
    timer.start(timerTick);
}

void GameField::onTest()
{
    shuffleCaps();
}

void GameField::mousePressEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();
    int x = pos.x() / ballSize.width();
    int y = pos.y() / ballSize.height();
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
            firstBall->SetSelected(false);
            secondBall = &balls[x][y];
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
                    if (swapCounter == ballSize.width())
                    {
                        swapCounter = 0;
                        ballsSwapTimer.stop();
                        disconnect(&ballsSwapTimer, &QTimer::timeout, this, nullptr);
                        firstBall->SetRect(QRect(firstBall->GetPos().x() * ballSize.width() + ballGap, firstBall->GetPos().y() * ballSize.height() + ballGap,
                            ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
                        secondBall->SetRect(QRect(secondBall->GetPos().x() * ballSize.width() + ballGap, secondBall->GetPos().y() * ballSize.height() + ballGap,
                            ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
                        Ball::Type tmp = firstBall->GetType();
                        firstBall->SetType(secondBall->GetType());
                        secondBall->SetType(tmp);
                        // 
                        QList<QList<QPoint>> shapes = getLineShapes(getShapes(balls));
                        if (!shapes.empty())
                            removeBalls(shapes);
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
                                    if (swapCounter == ballSize.width())
                                    {
                                        swapCounter = 0;
                                        ballsSwapTimer.stop();
                                        disconnect(&ballsSwapTimer, &QTimer::timeout, this, nullptr);
                                        firstBall->SetRect(QRect(firstBall->GetPos().x() * ballSize.width() + ballGap, firstBall->GetPos().y() * ballSize.height() + ballGap,
                                            ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
                                        secondBall->SetRect(QRect(secondBall->GetPos().x() * ballSize.width() + ballGap, secondBall->GetPos().y() * ballSize.height() + ballGap,
                                            ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
                                        Ball::Type tmp = firstBall->GetType();
                                        firstBall->SetType(secondBall->GetType());
                                        secondBall->SetType(tmp);
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
    }
    // for debug purposes
    else if (e->button() == Qt::RightButton)
    {
        balls[x][y].SetType(Ball::ExtraBonus2);
        removeBalls(getLineShapes(getShapes(balls)));
    }
}
void GameField::mouseDoubleClickEvent(QMouseEvent* e)
{
    // TODO: fix bug with decreasing score
    QPoint pos = e->pos();
    int x = pos.x() / ballSize.width();
    int y = pos.y() / ballSize.height();
    balls[x][y].SetSelected(false);
    QList<QPoint> shape;
    if (balls[x][y].GetType() == Ball::Bonus4)
    {
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
    }
    else if (balls[x][y].GetType() == Ball::Bonus5)
    {
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
    }
    else if (balls[x][y].GetType() == Ball::Bonus6)
    {
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
                if (x < fieldSize.width() - 2)
                    shape << QPoint(x + 3, j);
            }
        }
    }
    else if (balls[x][y].GetType() == Ball::ExtraBonus1)
    {
        // TODO: implement
    }
    else if (balls[x][y].GetType() == Ball::ExtraBonus2)
    {
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
            extraBonus2Pos << QPoint(x * ballSize.width(), y * ballSize.height());

        for (int i = 0; i < shapes.size(); i++)
            extraBonus2Lines << QLine(
                (x + 0.5) * ballSize.width(),
                (y + 0.5) * ballSize.width(),
                (x + 0.5) * ballSize.height(),
                (y + 0.5) * ballSize.height());

        isExtraBonus2Pos = true;
        int maxSize = qMax(shapes[0].size(), qMax(shapes[1].size(), qMax(shapes[2].size(), shapes[3].size())));
        int step = 2;
        extraBonus2Pen = QPen(QColor(255, 128, 0, 128), ballSize.width() / 10);
        isExtraBonus2Lines = true;

        connect(&removeTimer, &QTimer::timeout, this, [=]
        {
                if (extraBonus2Pos[0].x() > 0)
                {
                    extraBonus2Pos[0] += QPoint(-step, 0);
                    extraBonus2Lines[0].setP2(QPoint(
                        extraBonus2Lines[0].p2().x() - step,
                        extraBonus2Lines[0].p2().y()));
                }
                if (extraBonus2Pos[1].x() < (fieldSize.width() - 1) * ballSize.width())
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
                if (extraBonus2Pos[3].y() < (fieldSize.height() - 1) * ballSize.height())
                {
                    extraBonus2Pos[3] += QPoint(0, step);
                    extraBonus2Lines[3].setP2(QPoint(
                        extraBonus2Lines[3].p2().x(),
                        extraBonus2Lines[3].p2().y() + step));
                }
            removeCounter++;
            if (removeCounter == maxSize * ballSize.width() / step)
            {
                removeTimer.stop();
                disconnect(&removeTimer, &QTimer::timeout, this, nullptr);
                removeCounter = 0;

                extraBonus2Pen = QPen(QColor(255, 128, 0), 1, Qt::PenStyle::SolidLine, Qt::RoundCap);
                connect(&removeTimer, &QTimer::timeout, this, [=]
                {
                    extraBonus2Pen.setWidth(extraBonus2Pen.width() + 1);
                    removeCounter++;
                    if (removeCounter == ballSize.width() / 2)
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
                        removeBalls(removeShapes);
                    }
                });
                removeTimer.start(timerTick);
            }
        });
        removeTimer.start(timerTick);
    }

    else if (balls[x][y].GetType() == Ball::ExtraBonus3)
    {
        // TODO: implement
    }
    //shapes << shape;
    //removeBalls(shapes);
}

void GameField::updateGameField()
{
    QPixmap pixmap(fieldSize.width() * ballSize.width(), fieldSize.height() * ballSize.height());
    pixmap.fill(Qt::white);
    QPainter p(&pixmap);
    p.drawImage(QPoint(0, 0), background);
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
    // draw extra bonus 2
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
void GameField::removeBalls(QList<QList<QPoint>>& shapes)
{
    if (!shapes.isEmpty())
    {
        sounds[Sound::RemoveBalls]->play();
        // update score
        for (int i = 0; i < shapes.size(); i++)
        {
            // 3 - 100
            // 4 - 200
            // 5 - 300
            score += (shapes[i].size() - 2) * 100;
        }
        emit scoreChanged(score);
        // add bonuses
        QVector<int> bonuses(3);
        for (int i = 0; i < shapes.size(); i++)
        {
            if (shapes[i].size() == 4)
                bonuses[0]++;
            else if (shapes[i].size() == 5)
                bonuses[1]++;
            else if (shapes[i].size() > 5)
                bonuses[2]++;
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
            if (removeCounter == (ballSize.width() - ballGap * 2) / 2)
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
                        dropCounter = ballSize.width();
                    if (dropCounter == ballSize.width())
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
                                    ball->SetRect(QRect(ball->GetPos().x() * ballSize.width() + ballSize.width() / 2,
                                        ball->GetPos().y() * ballSize.height() + ballSize.height() / 2, 0, 0));
                                else
                                    ball->SetRect(QRect(ball->GetPos().x() * ballSize.width() + ballGap, ball->GetPos().y() * ballSize.height() + ballGap,
                                        ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
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
                            if (fillCounter == (ballSize.width() - ballGap * 2) / 2)
                            {
                                fillCounter = 0;
                                fillTimer.stop();
                                disconnect(&fillTimer, &QTimer::timeout, this, nullptr);
                                removeBalls(getLineShapes(getShapes(balls)));
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
    QList<PossibleMove> moves = getPossibleMoves();
    // shuffle caps is there are no possible moves
    if (moves.isEmpty())
    {
        shuffleCaps();
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
    QImage image(ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter p(&image);
    r.render(&p, QRectF(0, 0, ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
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
            // emulate caps exchange
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
                PossibleMove move(MoveType::Cap, QPoint(i, j), QPoint(i + 1, j), maxShape.size());
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
            // emulate caps exchange
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
                PossibleMove move(MoveType::Cap, QPoint(i, j), QPoint(i, j + 1), maxShape.size());
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
void GameField::shuffleCaps()
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
