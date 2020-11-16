#include "GameField.h"

GameField::GameField(QWidget* parent)
    : QLabel(parent)
{
    ui.setupUi(this);

    qsrand(2);
    // allocate game field data
    balls.resize(fieldSize.width());
    for (int i = 0; i < fieldSize.width(); ++i)
        balls[i].resize(fieldSize.height());
    // fill game field rectangles and positions
    for (int i = 0; i < fieldSize.width(); ++i)
    {
        for (int j = 0; j < fieldSize.height(); ++j)
        {
            balls[i][j].SetRect(QRect(i * ballSize.width() + ballGap, j * ballSize.height() + ballGap,
                ballSize.width() - ballGap * 2, ballSize.height() - ballGap * 2));
            balls[i][j].SetPos(QPoint(i, j));
        }
    }
    // generate field with no moves
    QList<QList<QPoint>> shapes;
    do
    {
        // fill game field types
        for (int i = 0; i < fieldSize.width(); ++i)
        {
            for (int j = 0; j < fieldSize.height(); ++j)
            {
                balls[i][j].SetType(Ball::Type(qrand() % Ball::GetBallNumber()));
            }
        }
        shapes = getLineShapes(getShapes());
    }
    while (!shapes.isEmpty());

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
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/7.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-1.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-2.svg"));
    textures << SvgToImage(QString(":/bonuses/Resources/bonuses/extra-3.svg"));
    selectedImage = SvgToImage(QString(":/misc/Resources/select.svg"));
    // load sounds
    sounds << new QSound(":/sounds/Resources/sounds/remove-balls.wav");
    sounds << new QSound(":/sounds/Resources/sounds/wrong-move.wav");
    sounds << new QSound(":/sounds/Resources/sounds/use-bonus-4.wav");

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
    removeBalls(getLineShapes(getShapes()));
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
                        QList<QList<QPoint>> shapes = getLineShapes(getShapes());
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
        balls[x][y].SetType(Ball::Bonus4);
        removeBalls(getLineShapes(getShapes()));
    }
}
void GameField::mouseDoubleClickEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();
    int x = pos.x() / ballSize.width();
    int y = pos.y() / ballSize.height();
    balls[x][y].SetSelected(false);
    QList<QList<QPoint>> shapes;
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
    }
    else if (balls[x][y].GetType() == Ball::Bonus5)
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

        if (x > 1)
            shape << QPoint(x - 2, y);
        if (x < fieldSize.width() - 2)
            shape << QPoint(x + 2, y);
        if (y > 1)
            shape << QPoint(x, y - 2);
        if (y < fieldSize.height() - 2)
            shape << QPoint(x, y + 2);

        if (x > 0 && y > 0)
            shape << QPoint(x - 1, y - 1);
        if (x < fieldSize.width() - 1 && y < fieldSize.height() - 1)
            shape << QPoint(x + 1, y + 1);
        if (x > 0 && y < fieldSize.height() - 1)
            shape << QPoint(x - 1, y + 1);
        if (x < fieldSize.width() - 1 && y > 0)
            shape << QPoint(x + 1, y - 1);

        if (x > 0 && y > 1)
            shape << QPoint(x - 1, y - 2);
        if (x > 1 && y > 0)
            shape << QPoint(x - 2, y - 1);
        if (x < fieldSize.width() - 1 && y < fieldSize.height() - 2)
            shape << QPoint(x + 1, y + 2);
        if (x < fieldSize.width() - 2 && y < fieldSize.height() - 1)
            shape << QPoint(x + 2, y + 1);
        if (x > 1 && y < fieldSize.height() - 1)
            shape << QPoint(x - 2, y + 1);
        if (x > 0 && y < fieldSize.height() - 2)
            shape << QPoint(x - 1, y + 2);
        if (x < fieldSize.width() - 2 && y > 0)
            shape << QPoint(x + 2, y - 1);
        if (x < fieldSize.width() - 1 && y > 1)
            shape << QPoint(x + 1, y - 2);
    }
    shapes << shape;
    removeBalls(shapes);
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
QList<QList<QPoint>> GameField::getShapes()
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
        int bonus4 = 0;
        for (int i = 0; i < shapes.size(); i++)
        {
            if (shapes[i].size() == 4)
                bonus4++;
        }
        for (int i = 0; i < bonus4; i++)
        {
            int x = qrand() % fieldSize.width();
            int y = qrand() % fieldSize.height();
            balls[x][y].SetType(Ball::Bonus4);
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
                        balls[p.x()][p.y()].SetType(Ball::Type::Empty);
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
                                removeBalls(getLineShapes(getShapes()));
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