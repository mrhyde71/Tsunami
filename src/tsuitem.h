#ifndef TSUITEM_H
#define TSUITEM_H

#include <cmath>       // needed for fmod
#include <string>      // needed for std::string
#include <sstream>     // needed for std::ostringstream
#include <map>         // needed for std::map

#include <QDebug>
#include <QColor>
#include <QtWidgets>
#include <QGraphicsItem>
#include <QMessageBox>
#include <qmath.h>

#include "tsuevents.h"


enum statusEnum {
    undefined,
    checking_files,
    downloading_metadata,
    downloading,
    finished,
    seeding,
    allocating,
    checking_resume_data,
    paused
};


class CByteValue
{
private:
    uint64_t m_value;

public:
    static constexpr uint64_t A_TERABYTE {1000000000000};
    static constexpr uint64_t A_GIGABYTE {1000000000};
    static constexpr uint64_t A_MEGABYTE {1000000};
    static constexpr uint64_t A_KILOBYTE {1000};
    static constexpr uint64_t A_BYTE     {1};

    static constexpr uint64_t A_TEBIBYTE {1099511627776}; // 0x10000000000 = 2^40
    static constexpr uint64_t A_GIBIBYTE {1073741824};    // 0x40000000 - 2^30
    static constexpr uint64_t A_MEBIBYTE {1048576};       // 0x100000 = 2^20
    static constexpr uint64_t A_KIBIBYTE {1024};          // 0x400 = 2^10

    enum class decimalUnit_t : uint64_t
    {
        TB = A_TERABYTE,
        GB = A_GIGABYTE,
        MB = A_MEGABYTE,
        kB = A_KILOBYTE,
        B  = A_BYTE
    };


    enum class binaryUnit_t : uint64_t
    {
        TiB = A_TEBIBYTE,
        GiB = A_GIBIBYTE,
        MiB = A_MEBIBYTE,
        KiB = A_KIBIBYTE,
        B   = A_BYTE
    };

public:
    explicit CByteValue(uint64_t v = 0) : m_value(v)
    {
    }


    static decimalUnit_t nearestDecimalUnit(uint64_t value_in_bytes)
    {
        if (value_in_bytes >= A_TERABYTE) {
            return decimalUnit_t::TB;
        }
        if (value_in_bytes >= A_GIGABYTE) {
            return decimalUnit_t::GB;
        }
        if (value_in_bytes >= A_MEGABYTE) {
            return decimalUnit_t::MB;
        }
        if (value_in_bytes >= A_KILOBYTE) {
            return decimalUnit_t::kB;
        }
        return decimalUnit_t::B;
    }

    static binaryUnit_t nearestBinaryUnit(uint64_t value_in_bytes)
    {
        if (value_in_bytes >= A_TEBIBYTE) {
            return binaryUnit_t::TiB;
        }
        if (value_in_bytes >= A_GIBIBYTE) {
            return binaryUnit_t::GiB;
        }
        if (value_in_bytes >= A_MEBIBYTE) {
            return binaryUnit_t::MiB;
        }
        if (value_in_bytes >= A_KIBIBYTE) {
            return binaryUnit_t::KiB;
        }
        return binaryUnit_t::B;
    }

    static const std::string& decimalUnitLabel(decimalUnit_t dut)
    {
        static const std::map<decimalUnit_t, std::string> labels {
            {decimalUnit_t::B, "B"},
            {decimalUnit_t::kB, "kB"},
            {decimalUnit_t::MB, "MB"},
            {decimalUnit_t::GB, "GB"},
            {decimalUnit_t::TB, "TB"},
        };

        return labels.at(dut);
    }


    static const std::string& binaryUnitLabel(binaryUnit_t but)
    {
        static const std::map<binaryUnit_t, std::string> labels {
            {binaryUnit_t::B, "B"},
            {binaryUnit_t::KiB, "KiB"},
            {binaryUnit_t::MiB, "MiB"},
            {binaryUnit_t::GiB, "GiB"},
            {binaryUnit_t::TiB, "TiB"},
        };

        return labels.at(but);
    }

    static double to_decimalValue(decimalUnit_t du, uint64_t value)
    {
        if (du == decimalUnit_t::B) {
            return (double) value;
        }
        return (static_cast<double>(value) / static_cast<double>(du));
    }

    static double to_binaryValue(binaryUnit_t bu, uint64_t value)
    {
        if (bu == binaryUnit_t::B) {
            return (double) value;
        }
        return (static_cast<double>(value) / static_cast<double>(bu));
    }

    const uint64_t& to_bytes() const
    {
        return m_value;
    }


    double to_decimalValue(decimalUnit_t du) const
    {
        return CByteValue::to_decimalValue(du, m_value);
    }

    double to_binaryValue(binaryUnit_t bu) const
    {
        return CByteValue::to_binaryValue(bu, m_value);
    }


    double to_kilobyte() const
    {
        return to_decimalValue(decimalUnit_t::kB);
    }

    double to_megabyte() const
    {
        return to_decimalValue(decimalUnit_t::MB);
    }

    double to_gigabyte() const
    {
        return to_decimalValue(decimalUnit_t::GB);
    }

    double to_terabyte() const
    {
        return to_decimalValue(decimalUnit_t::TB);
    }


    double to_kibibyte() const
    {
        return to_binaryValue(binaryUnit_t::KiB);
    }

    double to_mebibyte() const
    {
        return to_binaryValue(binaryUnit_t::MiB);
    }

    double to_gibibyte() const
    {
        return to_binaryValue(binaryUnit_t::GiB);
    }

    double to_tebibyte() const
    {
        return to_binaryValue(binaryUnit_t::TiB);
    }

    static std::string toDecimalMetricString(uint64_t v)
    {
        // this is used to obtain a string representing the nearest multiple in SI (International System of Units)
        std::ostringstream o;

        decimalUnit_t ut = CByteValue::nearestDecimalUnit(v);

        o << static_cast<uint64_t>(CByteValue::to_decimalValue(ut, v)) << " " << CByteValue::decimalUnitLabel(ut);
        return o.str();
    }


    static std::string toBinaryMetricString(uint64_t v)
    {
        std::ostringstream o;

        binaryUnit_t ut = CByteValue::nearestBinaryUnit(v);

        o << static_cast<uint64_t>(CByteValue::to_binaryValue(ut, v)) << " " << CByteValue::binaryUnitLabel(ut);
        return o.str();
    }

    operator const uint64_t& () const
    {
        return m_value;
    }

};

class tsuItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    tsuItem();
    tsuItem(const std::string &head);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void set_FactorTransform(const qreal & value);
    qreal get_FactorTransform() const;
    const QString& get_Head() const;

    void set_Size(const uint64_t & value);
    void set_Downloaded(const uint64_t & value);
    void set_Uploaded(const uint64_t & value);
    uint64_t get_Downloaded() const;
    uint64_t get_Uploaded() const;

    statusEnum get_Status() const;
    void set_Status(const int &value);

    int get_RateDownload() const;
    void set_RateDownload(const int &value);

    int get_RateUpload() const;
    void set_RateUpload(const int &value);

    bool get_Visibility() const;
    void set_Visibility(const bool &visible);

    static const qreal ItemWidth;
    static const qreal ItemHeight;
    static const qreal ItemGlowRadius;

    const std::string& get_Hash() const;
    void set_Hash(const std::string &value);

    void executeItemRemove(bool const &filesToo);
    void executePause();
    void executeResume();

    QString convertSize(const uint64_t &size);
    static QString convertSizeUnit(const uint64_t &size);


public slots:
    void setValue(const tsuEvents::tsuEvent &event);
    void emitCancel();

private:
    int p_progressValue {};
    QString p_head;
    qreal p_factorTransform {1};
    bool p_isFadingOut {};
    bool p_isFadingIn {};

    bool p_isMoving {};

    QPixmap p_imageBkg {":/images/tsuitem/bkg.png"};
    QPixmap p_imageBkg2 {":/images/tsuitem/bkg2.png"};
    QPointF p_imageBkg2Position {1, 158};
    QPixmap p_imageExit {":/images/tsuitem/X.png"};
    QPointF p_imageExitPosition = QPointF(122, 4);
    QPixmap p_imagePause = QPixmap(":/images/tsuitem/pause.png");
    QPointF p_imagePausePosition = QPointF(31, 167);
    QPixmap p_imageResume = QPixmap(":/images/tsuitem/resume.png");
    QPointF p_imageResumePosition = QPointF(46, 167);

    QPixmap p_imageStateD {":/images/tsuitem/state_download.png"};
    QPixmap p_imageStateL {":/images/tsuitem/state_loading.png"};
    QPixmap p_imageStateP {":/images/tsuitem/state_pause.png"};
    QPixmap p_imageStateW {":/images/tsuitem/state_warning.png"};
    QPixmap p_imageStateF {":/images/tsuitem/finished.png"};
    QPixmap p_imageSelected {":/images/tsuitem/selected.png"};

    QPointF p_imageStatePosition {1, 158};
    QPointF p_imageStateFinishedPosition {16, 140};
    QPointF p_imageSelectedPosition {99, 148};

    int p_textFontHeight {12};
    int p_symbolFontHeight {7};
    int p_progress_width {10};
    qreal p_shadowDepth {1.5};
    qreal p_fontBorder {0.5};
    QColor p_colorText {255, 255, 255};
    QColor p_colorShadow {58, 58, 58};
    QFont p_fontText {"Bebas", p_textFontHeight};
    QFont p_fontSymbol {"Bebas", p_symbolFontHeight};
    QFont p_indicatorFont {"Tahoma", 8, QFont::Thin};
    QFont p_indicatorUnitFont {"Tahoma", 6, QFont::Thin};

    QRectF p_rectProgress {45.5, 69, 49.0, 49.0};
    QPen p_penProgress;
    QConicalGradient p_gradientProgress;
    int p_arcLengthApproximation {p_progress_width + p_progress_width / 3};

    QColor p_colorLabel {127,127,127};
    QFont p_fontLabel {"Tahoma", 7};

    QGraphicsDropShadowEffect *p_itemShadow{};
    QPropertyAnimation *p_itemShadowAnimation{};

    std::string p_hash;
    uint64_t p_size {};
    uint64_t p_downloaded{};
    uint64_t p_uploaded{};
    int p_rateDownload{};
    int p_rateUpload{};
    int p_numSeeds{};
    int p_numPeers{};
    statusEnum p_status {statusEnum::undefined};
    QDateTime p_eta;

    bool p_cancelFilesOnDelete {};

    bool p_visible {true};

private slots:
    void fadeInFinished();
    void fadeOutFinished();

protected:
    void createItem();
    QString remainingTime() const;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void cancelRequested(const std::string &hash, const bool &canelFiles);
    void pauseRequested(const std::string &hash);
    void resumeRequested(const std::string &hash);
    void detailsRequested(const std::string &hash);
    void downloadFinished(tsuItem *item);

};

#endif // TSUITEM_H
