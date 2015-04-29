#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_counter(0),
    m_paused(false)
{

    ui->setupUi(this);
    this->update();

    //Construct the buttons and connect them to the appropriate handler functions
    saveButton = new QPushButton(this);
    quitButton = new QPushButton(this);
    saveButton->setText("Save");
    quitButton->setText("Quit");
    connect(saveButton, SIGNAL(released()), this, SLOT(handleSaveButton()));
    connect(quitButton, SIGNAL(released()), this, SLOT(handleQuitButton()));

    //Construct the timer and connect to the nextFrame slot
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    m_timer->start(32);

    m_update_flag = false;

    //Construct the QLabel for displaying the character animation
    //Idk where this went??? But it's working...
}

Dialog::~Dialog()
{
    delete ui;
    delete saveButton;
    delete quitButton;

    //Added
    delete m_timer;
}

//Sets the game pointer to the game constructed by the builder
void Dialog::setGame(Game *game)
{
    m_game = game;
    setSize(m_game->getXDimension(), m_game->getYDimension());
    m_game->getPlayer()->beginPlayerAnimation(this);
}

void Dialog::setFileIO(fileIO *fileReader)
{
    m_fileIO = fileReader;
}

//Resizes the dialog box
void Dialog::setSize(int width, int height)
{
    this->resize(width, height);
}

//If the game is not paused, calls update()
void Dialog::nextFrame()
{  
    if (!m_paused) {
        saveButton->hide();
        quitButton->hide();
        update();
        m_update_flag = true;
//        m_update_flag = false;
    } else {
        saveButton->setGeometry(this->width()/3, this->height()/3, 100, 50);
        quitButton->setGeometry(2*this->width()/3 - 100, this->height()/3, 100, 50);
        saveButton->show();
        quitButton->show();
    }
}

void Dialog::handleSaveButton()
{
    m_fileIO->saveGame(m_game);
}

void Dialog::handleQuitButton()
{
    this->close();
}

//Handles the key events:
//'P' pauses the game
//'<' and '>' increment the player position
void Dialog::keyPressEvent(QKeyEvent *event)
{

    //Pause Event
    if (event->key() == Qt::Key_P && !m_paused) {
        m_paused = true;
        m_game->getPlayer()->pause(m_paused);
    } else if (event->key() == Qt::Key_P && m_paused) {
        m_paused = false;
        m_game->getPlayer()->pause(m_paused);
    }

    if (!m_paused) {
        if (event->key() == Qt::Key_Period) {
            m_game->getPlayer()->movePlayerX(2);
        } else if (event->key() == Qt::Key_Comma) {
            m_game->getPlayer()->movePlayerX(-2);
        } else if (event->key() == Qt::Key_Space) {
            m_game->getPlayer()->set_jumping(true);
//            m_game->getPlayer()->movePlayerY();
        }
    }
}


//Renders the background image
void Dialog::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    m_game->getBackground()->renderBackground(painter, !m_paused);
    m_game->getPlayer()->jump(&m_update_flag, m_counter, painter);
    m_counter++;
}
