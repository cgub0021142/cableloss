#include "widget.h"
#include "ui_widget.h"
#include <QTime>
#include <QMessageBox>
#include <QPointF>
#include <iostream>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    get_port_num_on_start(false),
    p1 (QStringList() << "port 1"),
    p1p5 (QStringList() << "port 1"<< "port 5"),
    p2_to_p4 (QStringList() << "port 2" << "port 3" << "port 4"),
    p6_to_p8 (QStringList() << "port 6" << "port 7" << "port 8"),
    measure_24xxMhz(true),
    measure_5xxxMhz(true),
    end_init(false)
{
    ui->setupUi(this);

//    ui->gb_setting->setStyleSheet(#gb_setting{
//        "border-image:url(\":/connection_sketch/setting_pic.png\")"});
    //get idx for recording data start point idx
    for( int idx = 0; idx < _countof(Measure_Channel); ++idx){
        if(Measure_Channel[idx] >= 2400){
            band_24g_start_idx = idx;
            break;
        }
    }

    for( int idx = band_24g_start_idx; idx < _countof(Measure_Channel); ++idx){
        if(Measure_Channel[idx] >= 2500){
            band_24g_end_idx = idx;
            break;
        }
    }

    for( int idx = band_24g_end_idx; idx < _countof(Measure_Channel); ++idx){
        if(Measure_Channel[idx] >= 5000){
            band_5g_start_idx = idx;
            break;
        }
    }

    vsa_model = new QStringListModel(this);
    ui->vsa_port->setModel(vsa_model);
    vsg_model = new QStringListModel(this);
    ui->vsg_port->setModel(vsg_model);

    ui->cbo_total_port->setItemData( 0, 2);
    ui->cbo_total_port->setItemData( 1, 4);
    ui->cbo_total_port->setItemData( 2, 8);
    ant_port = 0;

    //tcpSocket->abort();
    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost( ui->ip_destination->text().toAscii().data(), 5499);

    connected = tcpSocket->waitForConnected(1000);
    //connect through tcp wait for 1sec
    if( connected){
        tcpSocket->write("ROUTe:PORTs?\r\n");
        tcpSocket->waitForBytesWritten(100);
        tcpSocket->waitForReadyRead();

        QString str(tcpSocket->readAll());
        str.remove('\n');
        port_cnt = str.mid(2).toInt();

//        if(port_cnt == 2 || port_cnt == 4 || port_cnt == 8){
//            get_port_num_on_start = true;
//        }

//        if(get_port_num_on_start){
//            ui->label_total_port->setEnabled(false);
//            ui->cbo_total_port->setEnabled(false);
//        }

        //init port setting
        if(port_cnt == 8){

//            vsa_model = new QStringListModel(this);
//            ui->vsa_port->setModel(vsa_model);
            vsa_model->setStringList(p1p5);

            ui->cbo_total_port->setCurrentIndex(2);
//            vsa_port = 5;
//            vsg_port = 8;
        }
        //tcpSocket->close();
    }
    else {
        port_cnt = 4;
		vsa_model->setStringList(p1);
        ui->vsa_port->setItemData(0,1);
		vsg_model->setStringList(p2_to_p4);
        connect_fail_msg(QString("Can't connect to machine. "),1500);
    }

    //second combobox depend on first combobox
//    vsg_model = new QStringListModel(this);
//    ui->vsg_port->setModel(vsg_model);
//    vsg_model->setStringList(p2_to_p4);

    ui->vsa_port->setCurrentIndex(0);
    qDebug()<<"in to click.";
//    ui->vsg_port->setCurrentIndex(2);
    //////////////////////////////////////////////////

    //set the input constraint to fit ip format
    QString ipRange = "(?:[1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->ip_destination->setValidator(ipValidator);
    //////////////////////////////////////////////////

    //initiate table model
        tableModel = new StandardTable(_countof(Measure_Channel), 6*port_cnt, this);
    //tableDelegate = new TableDelegate(this);
    //ui->tableView->setItemDelegate(tableDelegate);


    //v header
    tableModel ->setVerticalHeaderItem(0, new QStandardItem(QString("Freq.")));
    for(int j =0; j< _countof(Measure_Channel); j++){
        tableModel ->setVerticalHeaderItem( j + 1, new QStandardItem(QString("%1").arg(Measure_Channel[j])));
    }


    //h header
    for(int i = 0; i < port_cnt; i++){
        HoriHeaderLabels << "Tx  - Rx"
                         << "REF"
                         << "ANT1"
                         << "ANT2"
                         << "ANT3"
                         << "ANT4" ;
        ui->tableView->setSpan(0 , 6 * i + 1, 1, 5);
//        tableModel->setItem( 1, 6*i, new QStandardItem(QString("Port%1").arg(i+1)));
        QStandardItem *port_data = new QStandardItem(QString("Port%1").arg(i+1));
        port_data->setTextAlignment(Qt::AlignCenter);
        QFont *font_size = new QFont();
        font_size->setPixelSize(20);
        port_data->setFont(*font_size);
        // port_data->setForeground();
        QBrush *background = new QBrush( QColor(200,200,200));
        port_data->setBackground(*background);
        tableModel->setItem( 0, 6*i + 1, port_data);
    }
    tableModel->setHorizontalHeaderLabels(HoriHeaderLabels);


    //read ini
    QString dir = "./debug/ADMFG_Configure/Port";
    QString extension = ".ini";
    if(!QFile(dir+QString::number(1)+extension).exists())
        dir = "./ADMFG_Configure/Port";


    tableModel->loss_data = new QMap<QString, float>[port_cnt];

    //總共8個檔案
    for(int port=0; port<port_cnt; port++){
        QSettings *file = new QSettings( dir+QString::number(port+1)+extension,QSettings::IniFormat);
        QStringList allKey = file->allKeys();

//        //save data in model (self-defined) private member
//        foreach(QString str, allKey){
//            //tableModel->loss_data.insert(str,file->value(str,999.999).toFloat());
//            tableModel->loss_data[port].insert(str,file->value(str,99.99).toFloat());
//        }

//        foreach(QString str, allKey){
//            qDebug() <<port <<" "<<
//                    str<< " "<< tableModel->loss_data[port].value(str,99.99);
//        }

        //從上到下把資料填入表格，填入ANT1~ANT4就好
        for( int col = 2; col < 6; col++){//1~6

            for( int row = 0; row < _countof(Measure_Channel); row++ ){
                QString key = HoriHeaderLabels[col] + "/" +QString::number(Measure_Channel[row]);
                    QStandardItem* loss = new QStandardItem(file->value(key).toString());
                    tableModel->setItem(row+1, (port)*6 + col, loss);
            }
         }
        delete file;
    }
    //combine to view and set cell width
    ui->tableView->setModel(tableModel);
    check_measured_ref = QVector<bool>(32,false);
//    check_measured_ref.resize(32);
    end_init = true;
}

Widget::~Widget()
{
    delete ui;
}



void Widget::change_sketch(){
    QString pic_name;
    if(ant_port == 0){
        pic_name = QString("%1_%2-%3.png").arg(port_cnt).arg(vsa_port).arg(vsg_port);
    }
    else{
        pic_name = QString("%1_%2-%3-%4.png").arg(port_cnt).arg(vsa_port).arg(vsg_port).arg(ant_port);
    }
    qDebug()<<"pic_name"<<pic_name;
    ui->gb_connection->setStyleSheet(QString("border-image:url(\":/connection_sketch/%1\");").arg(pic_name));
}

void Widget::on_btn_save_all_port_loss_clicked()
{
    this->setDisabled(true);
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>( ui->tableView->model());
    QString textData;
    int rows = model->rowCount();
    int columns = model->columnCount();

    // [Save all data to csv file] (header file <QFile> needed)
    // for debug use
    // machine only needs ini files
    //extract data to string, then save to csv
    //row0
    for(int port =1;port<port_cnt;port++)
        textData += "freq,port"+QString::number(port)+",ref,ant1,ant2,ant3,ant4,";
    textData+="\n";
    //row1~
    for (int i = 1; i < rows; i++) {
        textData += QString::number(Measure_Channel[i-1])+",";
        for (int j = 0; j < columns; j++) {
                textData += model->data(model->index(i,j)).toString();
                textData += ", "  ;    // for .csv file format
        }
        textData += "\n";             // (optional: for new line segmentation)
    }


    QFile csvFile("result.csv");
    if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

        QTextStream out(&csvFile);
        out << textData;

        csvFile.close();
    }
    //save as ini for future usage
    for(int port =0; port<port_cnt;port++)    {

        QSettings ini_file(".\\Port"+QString::number(port+1)+".ini",QSettings::IniFormat);

        for (int i = 1; i < rows; i++) {

            for (int j = 1; j <= 4; j++) {

                ini_file.setValue("ANT"+QString::number(j)+"/"+QString::number(Measure_Channel[i-1]),
                                  model->data(model->index(i,j+6*port+1)).toString());

            }
        }
    }


    QMessageBox msg_box;
    msg_box.setWindowTitle(" ");
    msg_box.setText("<font size = 20 > save completed. </font>");
    msg_box.setIcon(QMessageBox::Information);
    msg_box.setStandardButtons(QMessageBox::Ok);
    QTimer::singleShot(700, &msg_box,SLOT(close()));
//    msg_box.button(QMessageBox::Ok)->animateClick(700);
    msg_box.exec();
    this->setDisabled(false);

}

void Widget::on_btn_measure_loss_clicked()
{
    //if()
    tcpSocket->abort();
    tcpSocket->connectToHost( ui->ip_destination->text().toAscii().data(), 5499);
    connected = tcpSocket->waitForConnected(1000);

    if(connected){
        //check port cnt is match from the gui if not match return
        tcpSocket->write("ROUTe:PORTs?\r\n");
        tcpSocket->waitForBytesWritten(100);
        tcpSocket->waitForReadyRead();

        QString str(tcpSocket->readAll());
        str.remove('\n');
        int port_cnt_from_machine = str.mid(2).toInt();
        if(port_cnt != port_cnt_from_machine){
            connect_fail_msg(QString("Total port setting in gui is wrong should change to %1port.").arg(port_cnt_from_machine),30000);
            return;
        }

        //check if ref is measured before measure the ant cable loss
        if(ant_port){
            if(!check_measured_ref[vsg_port]){
                connect_fail_msg(QString("Measure ref first."));
                return;
            }
        }else
            check_measured_ref[vsg_port] = true;


        char *preset_cmd []= {
            "CONFigure:USER:REGister?\r\n",
            "CONFigure:TRIGger:TYPE IMMediate\r\n",
            "OUTPut:POWer:LEVel -10\r\n",
            "INPut:CAPTure:TIME  0.001\r\n",
            "INPut:TXQuality:SKIP:COUNt?\r\n",
            "INPut:TXQuality:SKIP:COUNt 5\r\n",
            "GPRF\r\n",
            "ROUTe:PORT:RESource RF1,OFF\r\n",
            "ROUTe:PORT:RESource RF2,OFF\r\n",
            "ROUTe:PORT:RESource RF3,OFF\r\n",
            "ROUTe:PORT:RESource RF4,OFF\r\n",
            "ROUTe:PORT:RESource RF5,OFF\r\n",
            "ROUTe:PORT:RESource RF6,OFF\r\n",
            "ROUTe:PORT:RESource RF7,OFF\r\n",
            "ROUTe:PORT:RESource RF8,OFF\r\n",
            "INPut:STARt\r\n"
        };
        for(int i = 0; i < _countof(preset_cmd); i++){
            tcpSocket->write(preset_cmd[i]);
            tcpSocket->waitForBytesWritten();
            tcpSocket->waitForReadyRead();
            tcpSocket->readAll();
        }

        //get port
        int cbo_vsa_index = ui->vsa_port->currentIndex();
        vsa_port = cbo_vsa_index * 4 + 1;
        vsg_port = vsa_port + ui->vsg_port->currentIndex() + 1;



        QByteArray port_setting [4]= {
            "ROUTe:PORT:RESource RF",
            "ROUTe:PORT:RESource RF",
            "CONFigure:RFCorrection ON\r\n",
            "INPut:LEVel:REFerence -10\r\n"
        };
        port_setting[0].append( QString::number(vsa_port)).append(",VSA\r\n");
        port_setting[1].append( QString::number(vsg_port)).append(",VSG\r\n");

        for(int i = 0; i < _countof(port_setting); ++i){
            tcpSocket->write(port_setting[i]);
            tcpSocket->waitForBytesWritten();
            tcpSocket->waitForReadyRead();
            tcpSocket->readAll();
        }

        QString measure_cmd [3] = {
            "INPut:FREQuency %1MHz\r\n",
            "CALCulate:TXQuality:ALL:SYNC\r\n",
            "FETCh:RFCorrection:RESource%1:LOSS?\r\n"};
        measure_cmd[2] = measure_cmd[2].arg(vsg_port);
        real_loss.clear();
        real_loss.reserve(120);
        if( measure_24xxMhz){
            for(int i = 2400, idx = 0; i < 2500; i += 10){
                tcpSocket->write(measure_cmd[0].arg(i).toAscii());
                tcpSocket->waitForBytesWritten();
                tcpSocket->waitForReadyRead();
                tcpSocket->readAll();

                tcpSocket->write(measure_cmd[1].toAscii());
                tcpSocket->waitForBytesWritten();
                tcpSocket->waitForReadyRead();
                tcpSocket->readAll();

                tcpSocket->write(measure_cmd[2].toAscii());
                tcpSocket->waitForBytesWritten();
                tcpSocket->waitForReadyRead();
                QString loss = QString(tcpSocket->readAll()).remove('\n').mid(2);
                loss_24xx[idx++] = loss;
                qDebug()<< "array data" <<idx<< loss_24xx[idx-1];
                tbloss_data tmp_rlt;
                tmp_rlt.Freq = i;
                tmp_rlt.Loss = loss.toFloat();
                real_loss.push_back(tmp_rlt);
            }
        }

        if( measure_5xxxMhz){
            for(int i = 5000, idx = 0; i < 6000; i += 10){
                tcpSocket->write(measure_cmd[0].arg(i).toAscii());
                tcpSocket->waitForBytesWritten();
                tcpSocket->waitForReadyRead();
                tcpSocket->readAll();

                tcpSocket->write(measure_cmd[1].toAscii());
                tcpSocket->waitForBytesWritten();
                tcpSocket->waitForReadyRead();
                tcpSocket->readAll();

                tcpSocket->write(measure_cmd[2].toAscii());
                tcpSocket->waitForBytesWritten();
                tcpSocket->waitForReadyRead();
                QString loss = QString(tcpSocket->readAll()).remove('\n').mid(2);
                loss_5xxx[idx++] = loss;
                qDebug()<< "array data" <<idx<< loss_24xx[idx-1];
                tbloss_data tmp_rlt;
                tmp_rlt.Freq = i;
                tmp_rlt.Loss = loss.toFloat();
                real_loss.push_back(tmp_rlt);
            }
        }
//        for(int i = 0; i < _countof(loss_24xx); ++i){
           // interpolation_loss();
//        }

//    for(int i = 0; i<100;i++)
//        qDebug()<<inter_polation_24xx[i];

        QByteArray end_loss_test_cmd[4]={
            "CONFigure:TRIGger:TYPE IMMediate\r\n",
            "INPut:TXQuality:SKIP:COUNt?\r\n",
            "INPut:TXQuality:SKIP:COUNt 5\r\n",
            "CONFigure:RFCorrection OFF\r\n"
        };

        for(int i = 0; i < 4; ++i){
            tcpSocket->write(end_loss_test_cmd[i]);
            tcpSocket->waitForBytesWritten();
            tcpSocket->waitForReadyRead();
            tcpSocket->readAll();
        }
        calc();
//        write_to_table_view();
    }
    else{
        connect_fail_msg(QString("Can't connect to machine. "),1500);
    }
}

void Widget::interpolation_loss(){
    if(measure_24xxMhz){
        for(int i = 0; i < 10; ++i){
            float span = (loss_24xx[i+1].toFloat() - loss_24xx[i].toFloat()) / 10;
            for(int idx = 0; idx < 10; ++idx){
                inter_polation_24xx[i*10+idx] = loss_24xx[i].toFloat() + (idx) * span;
            }
        }
    }
}

void Widget::write_to_table_view(){

}

void Widget::connect_fail_msg(QString &str, int msec){
    QMessageBox msg_box;
    msg_box.setWindowTitle(" ");
    msg_box.setText("<font size = 16 >" + str + "</font>");
    msg_box.setIcon(QMessageBox::NoIcon);
    QTimer::singleShot(msec, &msg_box,SLOT(close()));
    msg_box.exec();
}


void Widget::on_vsa_port_currentIndexChanged(int index)
{
    if(index == 0){
        vsg_model->setStringList(p2_to_p4);
    }
    else
        vsg_model->setStringList(p6_to_p8);

    vsa_port = index * 4 + 1;
    qDebug()<< "vsa" << vsa_port;
    ui->vsg_port->setCurrentIndex(2);
}

void::Widget::scroll_to_specified(){
    if(end_init){
        QModelIndex top_of_data = tableModel->index(0, vsg_port * 6 - 6 + ant_port + 1);
        qDebug()<< top_of_data;
        ui->tableView->setCurrentIndex(top_of_data);
        ui->tableView->scrollTo(top_of_data, QAbstractItemView::PositionAtTop);
        qDebug()<<ui->tableView->currentIndex();
    }
}

void Widget::on_vsg_port_currentIndexChanged(int index)
{
    vsg_port = vsa_port + 1 + ui->vsg_port->currentIndex();
    change_sketch();
    scroll_to_specified();
}

void Widget::on_ANT_num_currentIndexChanged(int index)
{
    ant_port = index;
    change_sketch();
    scroll_to_specified();
}

void Widget::on_cbo_total_port_currentIndexChanged(int index)
{
    port_cnt = ui->cbo_total_port->itemData(index).toInt();
    qDebug()<<"xxxport_cnt"<<port_cnt;
    switch(port_cnt){
    case 2:
        ui->vsa_port->setCurrentIndex(0);
        vsg_model->setStringList(p2_to_p4);
        ui->vsg_port->setCurrentIndex(0);
        ui->vsa_port->setDisabled(true);
        ui->vsg_port->setDisabled(true);
        break;
    case 4:
        vsa_model->setStringList(p1);
        vsg_model->setStringList(p2_to_p4);

        ui->vsa_port->setCurrentIndex(0);
        ui->vsa_port->setDisabled(false);
        ui->vsg_port->setDisabled(false);
        break;
    case 8:
        vsa_model->setStringList(p1p5);
        vsg_model->setStringList(p6_to_p8);
        ui->vsa_port->setCurrentIndex(0);
        ui->vsa_port->setDisabled(false);
        ui->vsg_port->setDisabled(false);
        break;

    }

}

double Widget::interpolation( double f0, double p0, double f1, double p1, double fi)
{
        double pi = 0.000;
        p1 = fabs(p1);
        p0 = fabs(p0);
        pi = ((fi-f0)/(f1-f0))*(p1-p0) + p0;
        if((f1-f0) == 0)
        {
                pi = p0;
        }
        pi = fabs(pi);
        return pi ;
}

//calc all interpolation data and write on table
//void Widget::ScanLossTbl(vector< tbloss_data > vec_Data , const std::string& filename, const std::string& MeaChannelstr, const std::string& Offset24g, const std::string& Offset5g)
void Widget::calc()
{

        double pi , f0 , f1 , p0 , p1;
        pi = f0 = f1 = p0 = p1 = 0.0;


        //////////////////count the quantity of the interpolated loss_data and init with offset///////////////////////
        int measure_idx = band_24g_start_idx;
        int measure_end_idx = _countof(Measure_Channel);

        if(!measure_5xxxMhz){
            measure_end_idx = band_24g_end_idx;
        }
        else if(!measure_24xxMhz){
            measure_idx = band_5g_start_idx;
        }

        int rlt_size = measure_end_idx - measure_idx;
		final_data.clear();
        final_data.resize(rlt_size);
        float offset_24xx = ui->offset_24xx->text().toFloat();
        float offset_5xxx = ui->offset_5xxx->text().toFloat();

        if(measure_24xxMhz && measure_5xxxMhz){
            int i = 0;
            for(; i < band_24g_end_idx - band_24g_start_idx; ++i){
                final_data[i] = offset_24xx;
            }
            for(; i < rlt_size; ++i){
                final_data[i] = offset_5xxx;
            }
        }
        else if(measure_24xxMhz && !measure_5xxxMhz){
            final_data.fill(offset_24xx);
        }
        else if(!measure_5xxxMhz){
            final_data.fill(offset_5xxx);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //interpolation
            for(int ref_idx =0, rlt_idx = 0; ref_idx< loss_ant.size() -1,

                measure_idx < measure_end_idx; ++measure_idx )
            {

                f0 = real_loss[ref_idx].Freq;

                f1 = real_loss[ref_idx + 1].Freq;

                int Freq = Measure_Channel[measure_idx];

                if( Freq >= f0 && Freq <= f1 )
                {
                        p0 = real_loss[ref_idx].Loss;

                        p1 = real_loss[ref_idx + 1].Loss;
						double tmp_debug = interpolation( f0, p0, f1, p1,  Freq );
                        final_data[rlt_idx++] +=  tmp_debug;
						qDebug()<<rlt_idx<<","<<tmp_debug;
                }
                else if(Freq > f1){
                        ref_idx++;
                        --measure_idx;
                }
                else if( Freq < f0){
//                        d.Loss = 0;
//                        vec_FinData.push_back(d);
                }

        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //write to table
//        QStandardItem* loss = new QStandardItem(file->value(key).toString());
//        tableModel->setItem(row+1, (port)*6 + col, loss);

        QVectorIterator<double> iter(final_data);
        //for(iter = final_data.begin(); iter !=final_data.end();++iter){
        int row;
        //int col = vsg_port *6 + ant_port + 1;
        int col = (vsg_port - 1) * 6 + ant_port + 1;
		int ref_col = (vsg_port - 1) * 6 + 1;
        if(measure_24xxMhz){
            row = band_24g_start_idx + 1;
        }else if(!measure_24xxMhz){
            row = band_5g_start_idx + 1;
        }
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>( ui->tableView->model());
        if(!ant_port){
            while(iter.hasNext()){
                QStandardItem* loss = new QStandardItem(QString::number(iter.next()));
                //y,x
                tableModel->setItem(row++, col, loss);
            }

        }else{
            while(iter.hasNext()){
                float ref_loss = model->data(model->index(row,ref_col)).toFloat();
                float ant_loss = iter.next();
                float real_ant_loss = ant_loss - ref_loss;
                qDebug()<< real_ant_loss;
                QStandardItem* loss = new QStandardItem(QString::number(real_ant_loss));
                //y,x
                tableModel->setItem(row++, col, loss);
            }
        }

        scroll_to_specified();
}


void Widget::on_band_select_currentIndexChanged(int index)
{
    switch(index){
    case 0:
        measure_24xxMhz = true;
        measure_5xxxMhz = true;
        break;
    case 1:
        measure_24xxMhz = true;
        measure_5xxxMhz = false;
        break;
    case 2:
        measure_24xxMhz = false;
        measure_5xxxMhz = true;
        break;
    }
}
