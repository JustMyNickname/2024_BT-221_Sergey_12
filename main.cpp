#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <memory>

class RecentFilesWidget : public QWidget{
public:
    RecentFilesWidget(QWidget *parent = nullptr) : QWidget(parent){
        QVBoxLayout *layout = new QVBoxLayout(this);
        recentFilesModel = std::make_unique<QStandardItemModel>(5, 1);
        recentFilesListView = new QListView(this);
        recentFilesListView->setModel(recentFilesModel.get());
        layout->addWidget(recentFilesListView);

        connect(recentFilesListView, &QListView::clicked, this, &RecentFilesWidget::openFile);
    }

    void addRecentFile(const QString &filePath){
        QStandardItem *item = new QStandardItem(filePath);
        recentFilesModel->insertRow(0, item);
    }

private slots:
    void openFile(const QModelIndex &index){
        QString filePath = recentFilesModel->itemFromIndex(index)->text();
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

private:
    std::unique_ptr<QStandardItemModel> recentFilesModel;
    QListView *recentFilesListView;
};

class GraphicsViewerApp : public QWidget{
public:
    GraphicsViewerApp(QWidget *parent = nullptr) : QWidget(parent){
        QVBoxLayout *layout = new QVBoxLayout(this);

        pathLineEdit = std::make_unique<QLineEdit>();
        layout->addWidget(pathLineEdit.get());

        recentFilesWidget = std::make_unique<RecentFilesWidget>();
        layout->addWidget(recentFilesWidget.get());

        connect(pathLineEdit.get(), &QLineEdit::returnPressed, this, &GraphicsViewerApp::searchFiles);

        loadPathsFromFile();
    }

    void searchFiles(){
        QString path = pathLineEdit->text();
        QDir directory(path);
        QStringList filters;
        filters << "*.jpeg" << "*.jpg" << "*.bmp" << "*.png";
        QStringList fileList = directory.entryList(filters, QDir::Files);

        for(const QString &file : fileList){
            QString filePath = directory.absoluteFilePath(file);
            recentFilesWidget->addRecentFile(filePath);
            savePathsToFile(filePath);
        }
    }

    void loadPathsFromFile(){
        QFile file("savedpaths.txt");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);
            while(!in.atEnd()){
                QString filePath = in.readLine();
                recentFilesWidget->addRecentFile(filePath);
            }
            file.close();
        }
    }

    void savePathsToFile(const QString &filePath){
        QFile file("savedpaths.txt");
        if(file.open(QIODevice::Append | QIODevice::Text)){
            QTextStream out(&file);
            out << filePath << "\n";
            file.close();
        }
    }

private:
    std::unique_ptr<QLineEdit> pathLineEdit;
    std::unique_ptr<RecentFilesWidget> recentFilesWidget;
};

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    std::unique_ptr<GraphicsViewerApp> graphicsViewerApp = std::make_unique<GraphicsViewerApp>();
    graphicsViewerApp->show();

    return app.exec();
}
