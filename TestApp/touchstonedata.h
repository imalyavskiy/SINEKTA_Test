#ifndef TOUCHSTONEDATA_H
#define TOUCHSTONEDATA_H

#include <optional>
#include <QAbstractListModel>
#include <QPoint>

class TouchstoneData : public QObject
{
    Q_OBJECT

public:
    explicit TouchstoneData(QObject *parent = nullptr);

    Q_INVOKABLE int size() const;

    Q_INVOKABLE void loadData(QString filePath, int viewportWidth, int viewportHeight, int epsilon);

    Q_INVOKABLE QPoint getPoint(int index);
signals:
    // signal intended to be connected in QML MUST follow lowerCamelCase style in C++
    // to connect in QML the signal MUST be mentioned in the same style but starting with 'on'
    // e.g. if 'fileLoadFailure' then 'onFileLoadFailure'
    void fileLoadFailure(const QString& filePath, const QString& reason);
    void fileLoadSuccess(const QString& filePath);

private:
    std::optional<std::ifstream> openFile(QString filePath);

    struct TouchstoneDataItem
    {
        double freq{};
        double s11_logmag{};
    };

    struct {
        struct {
            double min{};
            double max{};
        } freqRange, s11Range;
    }dataRect;

    struct FileReadResult{
        size_t lines_read{};
        size_t lines_skipped{};
        size_t lines_failed{};
    };

    std::optional<FileReadResult> readTouchstoneData(std::ifstream& data_file, std::vector<TouchstoneDataItem>& m_data);

    bool normalizeData(int left, int right, int bottom, int top);

    bool filterData(int epsilon);

    std::vector<TouchstoneDataItem> m_data;

    double Dist(const TouchstoneDataItem& left, const TouchstoneDataItem& right);

    bool validateData();
};


#endif // TOUCHSTONEDATA_H
