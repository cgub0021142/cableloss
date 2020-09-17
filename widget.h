#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QStringList>
#include <QStringListModel>
#include <QString>
//#include "tablemodel.h"
#include "standardtable.h"
#include "tabledelegate.h"
#include <QDebug>
#include <QStandardItemModel>
#include <QSettings>

#include <QVector>
#include <vector>
#include <string>
typedef struct Data
{
        double Freq;
        double Loss;
}tbloss_data;

static const int Measure_Channel[177]= {710,741,897,936,1575,
        2400,2401,2402,2403,2404,2405,2406,2407,2408,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2419,
        2420,2421,2422,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2433,2434,2435,2436,2437,2438,2439,
        2440,2441,2442,2443,2444,2445,2446,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2459,
        2460,2461,2462,2463,2464,2465,2466,2467,2468,2469,2470,2471,2472,2473,2474,2475,2476,2477,2478,2479,
        2480,2481,2482,2483,2484,
        5000,5010,5020,5030,5040,5050,5060,5070,5080,5090,5100,5110,5120,5130,5140,5150,5160,5170,5180,5190,
        5200,5210,5220,5230,5240,5250,5260,5270,5280,5290,5300,5310,5320,5330,5340,5350,5360,5370,5380,5390,
        5400,5410,5420,5430,5440,5450,5460,5470,5480,5490,5500,5510,5520,5530,5540,5550,5560,5570,5580,5590,
        5600,5610,5620,5630,5640,5650,5660,5670,5680,5690,5700,5710,
        5745,5755,5765,5775,5785,5795,5805,5815,5825,5835,5845,5855,5865,5875,5885};

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();


private:
    Ui::Widget *ui;
    QTcpSocket *tcpSocket;
    int port_cnt;
    bool connected;
    bool get_port_num_on_start;


    QStringListModel *vsa_model;
    QStringListModel *vsg_model;
    QStringList p1;
    QStringList p1p5;
    QStringList p2_to_p4;
    QStringList p6_to_p8;

//    QStandardItemModel *tableModel;
    StandardTable *tableModel;
    TableDelegate *tableDelegate;
    QStringList HoriHeaderLabels;
    void connect_fail_msg(QString &str, int msec = 3000);
    bool end_init;

    int band_24g_start_idx;
    int band_24g_end_idx;
    int band_5g_start_idx;
    int vsa_port;
    int vsg_port;
    int ant_port;
    bool measure_24xxMhz;
    bool measure_5xxxMhz;
    QString loss_24xx[10];
    QString loss_5xxx[100];
    QVector<tbloss_data> real_loss;
    QVector<tbloss_data> loss_ant;
    QVector<tbloss_data> loss_ref;
    float inter_polation_24xx[100];
    QString inter_polation_5xxx[1000];
    void interpolation_loss();
    void write_to_table_view();
    void change_sketch();
    double interpolation(double f0, double p0, double f1, double p1, double fi);
    void calc();
    QVector <double> final_data;
    void scroll_to_specified();
    QVector <bool> check_measured_ref;



private slots:
    void on_band_select_currentIndexChanged(int index);
    void on_cbo_total_port_currentIndexChanged(int index);
    void on_ANT_num_currentIndexChanged(int index);
    void on_vsg_port_currentIndexChanged(int index);
    void on_vsa_port_currentIndexChanged(int index);
    void on_btn_measure_loss_clicked();
    void on_btn_save_all_port_loss_clicked();
};

#endif // WIDGET_H
