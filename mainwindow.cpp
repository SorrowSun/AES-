#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QDebug>
#include <QtMath>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//二进制格式QString转QList<int>
QList<int>QStringBinary2QListInt(QString input)
{
    QList<int>result = {};
    for(int i = 0; i < input.length(); i++)
    {
        result.append(input[i].digitValue());
    }
    return result;
}

//转回QString
QString QListInt2QStringBinary(QList<int>input)
{
    QString result;
    QTextStream stream(&result);
    for(int i = 0; i < 16; i++)
    {
        stream << input[i];
    }
    stream.flush();
    return result;
}

//字符转换成二进制字符串8位
QString QChar2QStringBinary(QChar input)
{
    QString result;
    QTextStream stream(&result);
    int charValue = (unsigned char)input.toLatin1();
    for(int i = 7; i >= 0; i--)
    {
        int pow = qRound(qPow(2, i));
        if(charValue >= pow)
        {
            stream << "1";
            charValue -= pow;
        }
        else
        {
            stream << "0";
        }
    }
    stream.flush();
    return result;
}

//二进制字符串8位转换回字符
QString QStringBinary2QChar(QString input)
{
    int resultInt = 0;
    for(int i = 0; i < input.length(); i++)
    {
        if(input[i].digitValue() == 1)
            resultInt += qRound(qPow(2, 7-i));
    }
    QChar result = QChar(resultInt);
    return result;
}

//4位bit转int
int bit2int(QList<int>input)
{
    if(input.length() != 4)
    {
        qDebug() << "不是四位bit";
        return 0;
    }

    int result = 0;
    for(int i = 0; i < 4; i++)
    {
        if(input[i] == 1)
            result += qRound(qPow(2, 3 - i));
    }
    return result;
}

//int转4位bit
QList<int>int2bit(int input)
{
    QList<int>result = {};
    int num = input;
    for(int i = 3; i >= 0; i--)
    {
        if(num >= qRound(qPow(2, i)))
        {
            num -= qRound(qPow(2, i));
            result.append(1);
        }
        else
        {
            result.append(0);
        }
    }
    return result;
}

//异或
QList<int>XOR(QList<int>A, QList<int>B)
{
    if(A.length() != B.length())
    {
        qDebug() << "异或的某一步出现AB长度不等问题";
        return A;
    }
    //错误排除

    QList<int>result = {};
    for(int i = 0; i < A.length(); i++)
    {
        if(A[i] == B[i])
        {
            result.append(0);
        }
        else
        {
            result.append(1);
        }
    }
    return result;
    //异或进行
}

//异或重载
QString XOR(QString A, QString B)
{
    if(A.length() != B.length())
    {
        qDebug() << "异或的某一步出现AB长度不等问题";
        return A;
    }
    //错误排除

    QString result;
    QTextStream stream(&result);
    for(int i = 0; i < A.length(); i++)
    {
        if(A[i] == B[i])
        {
            stream << '0';
        }
        else
        {
            stream << '1';
        }
    }
    stream.flush();
    return result;
    //异或进行
}

//密钥拓展中的前后交换
QList<int>RotNib(QList<int>w)
{
    QList<int>result(w);
    int length = w.length();
    for(int i = 0; i < length; i++)
    {
        if(i < length/2)
        {
            result[i + length/2] = w[i];
        }
        else
        {
            result[i - length/2] = w[i];
        }
    }
    return result;
    //交换进行
}

//密钥拓展中的S盒替换
QList<int>SubNib(QList<int>w)
{
    if(w.length() % 4 != 0)
    {
        qDebug() << "密钥拓展S盒替换中出现位数不能整除于4问题";
        return w;
    }
    //错误排除

    QList<int>S = {9, 4, 10, 11, 13, 1, 8, 5, 6, 2, 0, 3, 12, 14, 15, 7};
    //S盒

    QList<int>result = {};
    double index_double = 0;
    int index = 0;
    for(int i = 0; i < w.length(); i++)
    {
        if((i % 4 == 0 && i != 0))
        {
            index = qRound(index_double);
            int numInS = S[index];

            for(int j = 0; j < 4; j++)
            {
                if(numInS >= qRound(qPow(2, 3-j)))
                {
                    numInS -= qRound(qPow(2, 3-j));
                    result.append(1);
                }
                else
                {
                    result.append(0);
                }
            }

            index_double = 0;
        }
        //每经过四个数后进行一次S盒转换，并将结果转换成二进制形式

        if(w[i] == 1)
            index_double += qPow(2, 3 - (i % 4));
        //累加

        if(i == w.length() - 1)
        {
            index = qRound(index_double);
            int numInS = S[index];

            for(int j = 0; j < 4; j++)
            {
                if(numInS >= qRound(qPow(2, 3-j)))
                {
                    numInS -= qRound(qPow(2, 3-j));
                    result.append(1);
                }
                else
                {
                    result.append(0);
                }
            }
        }
        //最后一次循环也进行一次
    }

    return result;
}


//密钥拓展
QList<int>appendKey(QList<int>key)
{
    QList<int>w0 = key.mid(0, 8);
    QList<int>w1 = key.mid(8, 8);

    QList<int>RCON1 = {1, 0, 0, 0, 0, 0, 0, 0};
    QList<int>w2 = XOR(w0, XOR(RCON1, SubNib(RotNib(w1))));

    QList<int>w3 = XOR(w2, w1);

    QList<int>RCON2 = {0, 0, 1, 1, 0, 0, 0, 0};
    QList<int>w4 = XOR(w2, XOR(RCON2, SubNib(RotNib(w3))));

    QList<int>w5 = XOR(w4, w3);

    QList<int>result = w0 + w1 + w2 + w3 + w4 + w5;
    return result;
}

//半字节代替
QList<int>NS(QList<int>input, QList<int>S)
{
    if(input.length() % 4 != 0)
    {
        qDebug() << "密钥拓展S盒替换中出现位数不能整除于4问题";
        return input;
    }
    //错误排除

    QList<int>result = {};
    double index_double = 0;
    int index = 0;
    for(int i = 0; i < input.length(); i++)
    {
        if((i % 4 == 0 && i != 0))
        {
            index = qRound(index_double);
            int numInS = S[index];

            for(int j = 0; j < 4; j++)
            {
                if(numInS >= qRound(qPow(2, 3-j)))
                {
                    numInS -= qRound(qPow(2, 3-j));
                    result.append(1);
                }
                else
                {
                    result.append(0);
                }
            }

            index_double = 0;
        }
        //每经过四个数后进行一次S盒转换，并将结果转换成二进制形式

        if(input[i] == 1)
            index_double += qPow(2, 3 - (i % 4));
        //累加

        if(i == input.length() - 1)
        {
            index = qRound(index_double);
            int numInS = S[index];

            for(int j = 0; j < 4; j++)
            {
                if(numInS >= qRound(qPow(2, 3-j)))
                {
                    numInS -= qRound(qPow(2, 3-j));
                    result.append(1);
                }
                else
                {
                    result.append(0);
                }
            }
        }
        //最后一次循环也进行一次
    }
    return result;
}

//行移位
QList<int>SR(QList<int>input)
{
    QList<int>result(input);
    int length = input.length();
    for(int i = length/2; i < length; i++)
    {
        if(i < 3*length/4)
        {
            result[i + length/4] = input[i];
        }
        else
        {
            result[i - length/4] = input[i];
        }
    }
    return result;
}

//列混淆
QList<int>MC(QList<int>input, QList<int>MCList)
{
    QList<QList<int>>MUL = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                            {0, 2, 4, 6, 8, 10, 12, 14, 3, 1, 7, 5, 11, 9, 15, 13},
                            {0, 3, 6, 5, 12, 15, 10, 9, 11, 8, 13, 14, 7, 4, 1, 2},
                            {0, 4, 8, 12, 3, 7, 11, 15, 6, 2, 14, 10, 5, 1, 13, 9},
                            {0, 5, 10, 15, 7, 2, 13, 8, 14, 11, 4, 1, 9, 12, 3, 6},
                            {0, 6, 12, 10, 11, 13, 7, 1, 5, 3, 9, 15, 14, 8, 2, 4},
                            {0, 7, 14, 9, 15, 8, 1, 6, 13, 10, 3, 4, 2, 5, 12, 11},
                            {0, 8, 3, 11, 6, 14, 5, 13, 12, 4, 15, 7, 10, 2, 9, 1},
                            {0, 9, 1, 8, 2, 11, 3, 10, 4, 13, 5, 12, 6, 15, 7, 14},
                            {0, 10, 7, 13, 14, 4, 9, 3, 15, 5, 8, 2, 1, 11, 6, 12},
                            {0, 11, 5, 14, 10, 1, 15, 4, 7, 12, 2, 9, 13, 6, 8, 3},
                            {0, 12, 11, 7, 5, 9, 14, 2, 10, 6, 1, 13, 15, 3, 4, 8},
                            {0, 13, 9, 4, 1, 12, 8, 5, 2, 15, 11, 6, 3, 14, 10, 7},
                            {0, 14, 15, 1, 13, 3, 2, 12, 9, 7, 6, 8, 4, 10, 11, 5},
                            {0, 15, 13, 2, 9, 6, 4, 11, 1, 14, 12, 3, 8, 7, 5, 10}};

    QList<int>s00 = input.mid(0, 4);
    QList<int>s01 = input.mid(4, 4);
    QList<int>s10 = input.mid(8, 4);
    QList<int>s11 = input.mid(12, 4);
    //转换前

    QList<int>S00 = XOR(int2bit(MUL[bit2int(s00)][MCList[0]]), int2bit(MUL[bit2int(s10)][MCList[1]]));
    QList<int>S01 = XOR(int2bit(MUL[bit2int(s01)][MCList[0]]), int2bit(MUL[bit2int(s11)][MCList[1]]));
    QList<int>S10 = XOR(int2bit(MUL[bit2int(s00)][MCList[2]]), int2bit(MUL[bit2int(s10)][MCList[3]]));
    QList<int>S11 = XOR(int2bit(MUL[bit2int(s01)][MCList[2]]), int2bit(MUL[bit2int(s11)][MCList[3]]));
    //转换后

    QList<int>result = S00 + S01 + S10 + S11;
    return result;
}

//二进制加密
QString plain2SipherBinary(QString keyText, QString plainText)
{
    QList<int>key = QStringBinary2QListInt(keyText);
    QList<int>plain = QStringBinary2QListInt(plainText);
    //转换成QList形式

    QList<int>keyAppended = appendKey(key);
    //密钥拓展成48位

    QList<int>S = {9, 4, 10, 11, 13, 1, 8, 5, 6, 2, 0, 3, 12, 14, 15, 7};
    //S盒

    QList<int>MCList = {1, 4, 4, 1};
    //列混淆矩阵

    QList<int>key0 = keyAppended.mid(0, 16);
    QList<int>key1 = keyAppended.mid(16, 16);
    QList<int>key2 = keyAppended.mid(32, 16);
    QList<int>result_QList = XOR(SR(NS(XOR(MC(SR(NS(XOR(plain, key0), S)), MCList), key1), S)), key2);
    //转换原理见公式

    QString result = QListInt2QStringBinary(result_QList);

    return result;
}

//二进制解密
QString sipher2PlainBinary(QString keyText, QString sipherText)
{
    QList<int>key = QStringBinary2QListInt(keyText);
    QList<int>sipher = QStringBinary2QListInt(sipherText);
    //转换成QList形式

    QList<int>keyAppended = appendKey(key);
    //密钥拓展成48位

    QList<int>SReverse = {10, 5, 9, 11, 1, 7, 8, 15, 6, 0, 2, 3, 12, 4, 13, 14};
    //S逆盒

    QList<int>MCListReverse = {9, 2, 2, 9};
    //列混淆逆矩阵

    QList<int>key0 = keyAppended.mid(0, 16);
    QList<int>key1 = keyAppended.mid(16, 16);
    QList<int>key2 = keyAppended.mid(32, 16);
    QList<int>result_QList = XOR(NS(SR(MC(XOR(NS(SR(XOR(sipher, key2)), SReverse), key1), MCListReverse)), SReverse), key0);
    //转换原理见公式

    QString result = QListInt2QStringBinary(result_QList);

    return result;
}

//ascii加密
QString plain2SipherAscii(QString keyText, QString plainText)
{
    QString result;
    QTextStream stream(&result);
    for(int i = 0; i < plainText.length()/2; i++)
    {
        QString binaryPlainText = QChar2QStringBinary(plainText[2*i]) + QChar2QStringBinary(plainText[2*i+1]);
        //两个字符转换成的二进制字符串拼接起来的16位明文字符串
        QString binarySipherText = plain2SipherBinary(keyText, binaryPlainText);
        //两个字符转换成的二进制字符串拼接起来的16位密文字符串
        stream << QStringBinary2QChar(binarySipherText.mid(0, 8)) << QStringBinary2QChar(binarySipherText.mid(8, 8));
        //输入到结果字符串里
    }
    stream.flush();
    return result;
}

//ascii解密
QString sipher2PlainAscii(QString keyText, QString sipherText)
{
    QString result;
    QTextStream stream(&result);
    for(int i = 0; i < sipherText.length()/2; i++)
    {
        QString binarySipherText = QChar2QStringBinary(sipherText[2*i]) + QChar2QStringBinary(sipherText[2*i+1]);
        //两个字符转换成的二进制字符串拼接起来的16位密文字符串
        QString binaryPlainText = sipher2PlainBinary(keyText, binarySipherText);
        //两个字符转换成的二进制字符串拼接起来的16位明文字符串
        stream << QStringBinary2QChar(binaryPlainText.mid(0, 8)) << QStringBinary2QChar(binaryPlainText.mid(8, 8));
        //输入到结果字符串里
    }
    stream.flush();
    return result;
}

//CBC加密
QString plain2SipherCBC(QString keyText, QString plainText, QString IVText)
{
    QString result;
    QTextStream stream(&result);
    QString lastSipherText = IVText;
    for(int i = 0; i < plainText.length()/2; i++)
    {
        QString binaryPlainText = QChar2QStringBinary(plainText[2*i]) + QChar2QStringBinary(plainText[2*i+1]);
        //两个字符转换成的二进制字符串拼接起来的16位明文字符串
        binaryPlainText = XOR(binaryPlainText, lastSipherText);
        //当前的字符串与上一次加密结果（第一个字符分组是与IV）异或
        QString binarySipherText = plain2SipherBinary(keyText, binaryPlainText);
        //得出加密结果
        lastSipherText = binarySipherText;
        //更新上一次加密结果
        stream << QStringBinary2QChar(binarySipherText.mid(0, 8)) << QStringBinary2QChar(binarySipherText.mid(8, 8));
        //输入到结果字符串里
    }
    stream.flush();
    return result;
}

//CBC解密
QString sipher2PlainCBC(QString keyText, QString sipherText, QString IVText)
{
    QString result;
    QTextStream stream(&result);
    QString lastSipherText = IVText;
    for(int i = 0; i < sipherText.length()/2; i++)
    {
        QString binarySipherText = QChar2QStringBinary(sipherText[2*i]) + QChar2QStringBinary(sipherText[2*i+1]);
        //两个字符转换成的二进制字符串拼接起来的16位密文字符串
        QString binaryPlainText = sipher2PlainBinary(keyText, binarySipherText);
        //得出解密结果
        binaryPlainText = XOR(binaryPlainText, lastSipherText);
        //当前的字符串与上一次加密结果（第一个字符分组是与IV）异或
        lastSipherText = binarySipherText;
        //更新上一次加密结果
        stream << QStringBinary2QChar(binaryPlainText.mid(0, 8)) << QStringBinary2QChar(binaryPlainText.mid(8, 8));
        //输入到结果字符串里
    }
    stream.flush();
    return result;
}

void MainWindow::on_pushButton_binaryPlain2Sipher_clicked()
{
    QString keyText = ui->lineEdit_binaryKey->text();
    QString plainText = ui->lineEdit_binaryPlain->text();
    //获取密钥和明文

    if(plainText.length() != 16 || keyText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(keyText).hasMatch()) && (regex.match(plainText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QString sipherText = plain2SipherBinary(keyText, plainText);
    ui->lineEdit_binarySipher->setText(sipherText);
    //显示
}


void MainWindow::on_pushButton_binarySipher2Plain_clicked()
{
    QString keyText = ui->lineEdit_binaryKey->text();
    QString sipherText = ui->lineEdit_binarySipher->text();

    if(sipherText.length() != 16 || keyText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(keyText).hasMatch()) && (regex.match(sipherText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QString plainText = sipher2PlainBinary(keyText, sipherText);
    ui->lineEdit_binaryPlain->setText(plainText);
    //显示
}


void MainWindow::on_pushButton_asciiPlain2Sipher_clicked()
{
    QString keyText = ui->lineEdit_asciiKey->text();
    QString plainText = ui->lineEdit_asciiPlain->text();
    //获取密钥和明文

    if(keyText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    if(plainText.length() % 2 != 0)
    {
        QMessageBox::information(nullptr, "提示", "明文长度不是双数", QMessageBox::Ok);
        return;
    }
    //明文长度不符合警告

    QRegularExpression regex_binary("^[01]*$");
    bool isBinary = regex_binary.match(keyText).hasMatch();
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QRegularExpression regex_ascii("^[\\x00-\\xFF]*$");
    bool isAscii = regex_ascii.match(plainText).hasMatch();
    if(!isAscii)
    {
        QMessageBox::information(nullptr, "提示", "密文不在ascii码表中", QMessageBox::Ok);
        return;
    }
    //非ascii码警告

    QString sipherText = plain2SipherAscii(keyText, plainText);
    ui->lineEdit_asciiSipher->setText(sipherText);
    //显示
}


void MainWindow::on_pushButton_asciiSipher2Plain_clicked()
{
    QString keyText = ui->lineEdit_asciiKey->text();
    QString sipherText = ui->lineEdit_asciiSipher->text();
    //获取密钥和明文

    if(keyText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    if(sipherText.length() % 2 != 0)
    {
        QMessageBox::information(nullptr, "提示", "明文长度不是双数", QMessageBox::Ok);
        return;
    }
    //明文长度不符合警告

    QRegularExpression regex_binary("^[01]*$");
    bool isBinary = regex_binary.match(keyText).hasMatch();
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QRegularExpression regex_ascii("^[\\x00-\\xFF]*$");
    bool isAscii = regex_ascii.match(sipherText).hasMatch();
    if(!isAscii)
    {
        QMessageBox::information(nullptr, "提示", "密文不在ascii码表中", QMessageBox::Ok);
        return;
    }
    //非ascii码警告

    QString plainText = sipher2PlainAscii(keyText, sipherText);
    ui->lineEdit_asciiPlain->setText(plainText);
    //显示
}


void MainWindow::on_pushButton_doublePlain2Sipher_clicked()
{
    QString keyText = ui->lineEdit_doubleKey->text();
    QString plainText = ui->lineEdit_doublePlain->text();
    //获取密钥和明文

    if(plainText.length() != 16 || keyText.length() != 32)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(keyText).hasMatch()) && (regex.match(plainText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QString sipherText1 = plain2SipherBinary(keyText.mid(0, 16), plainText);
    QString sipherText2 = plain2SipherBinary(keyText.mid(16, 16), sipherText1);
    ui->lineEdit_doubleSipher->setText(sipherText2);
    //显示
}


void MainWindow::on_pushButton_doubleSipher2Plain_clicked()
{
    QString keyText = ui->lineEdit_doubleKey->text();
    QString sipherText = ui->lineEdit_doubleSipher->text();

    if(sipherText.length() != 16 || keyText.length() != 32)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(keyText).hasMatch()) && (regex.match(sipherText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QString plainText1 = sipher2PlainBinary(keyText.mid(16, 16), sipherText);
    QString plainText2 = sipher2PlainBinary(keyText.mid(0, 16), plainText1);
    ui->lineEdit_doublePlain->setText(plainText2);
    //显示
}


void MainWindow::on_pushButton_crack_clicked()
{
    ui->pushButton_crack->setEnabled(false);
    QString plainText = ui->lineEdit_doublePlain->text();
    QString sipherText = ui->lineEdit_doubleSipher->text();
    //获取明文和密文

    if(sipherText.length() != 16 || plainText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密文长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(plainText).hasMatch()) && (regex.match(sipherText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密文不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QMap<QString, QString>midTextMap;
    ui->plainTextEdit_crack->appendPlainText("开始破解......\n");
    for(unsigned int i = 0; i < 65536; i++)
    {
        QString keyText;
        QTextStream stream(&keyText);
        int currNum = i;
        for(int j = 0; j < 16; j++)
        {
            if(currNum >= qRound(qPow(2, 15-j)))
            {
                stream << '1';
                currNum -= qRound(qPow(2, 15-j));
            }
            else
            {
                stream << '0';
            }
        }
        stream.flush();
        //整数转换成16bit密钥

        QString midText = plain2SipherBinary(keyText, plainText);
        midTextMap[midText] = keyText;
        //写入map
    }
    //构建midText与key1的map

    ui->plainTextEdit_crack->appendPlainText("midTextMap构建完成......\n");

    for(unsigned int i = 0; i < 65536; i++)
    {
        QString keyText;
        QTextStream stream(&keyText);
        int currNum = i;
        for(int j = 0; j < 16; j++)
        {
            if(currNum >= qRound(qPow(2, 15-j)))
            {
                stream << '1';
                currNum -= qRound(qPow(2, 15-j));
            }
            else
            {
                stream << '0';
            }
        }
        stream.flush();
        if(i == 128)
            qDebug() << keyText;
        //整数转换成16bit密钥

        QString midText = sipher2PlainBinary(keyText, sipherText);
        if(midTextMap.value(midText, "0") != "0")
            ui->plainTextEdit_crack->appendPlainText("明文" + plainText + "与密文" + sipherText + "的密钥是\n" + midTextMap.value(midText) + keyText + "\n");
    }
    //再次遍历一遍，如果midText出现相等的态势，则输出此时的key1 + key2

    ui->plainTextEdit_crack->appendPlainText("破解结束......\n");
    ui->pushButton_crack->setEnabled(true);
}

void MainWindow::on_pushButton_triplePlain2Sipher_clicked()
{
    QString keyText = ui->lineEdit_tripleKey->text();
    QString plainText = ui->lineEdit_triplePlain->text();
    //获取密钥和明文

    if(plainText.length() != 16 || keyText.length() != 32)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(keyText).hasMatch()) && (regex.match(plainText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QString sipherText1 = plain2SipherBinary(keyText.mid(0, 16), plainText);
    QString sipherText2 = sipher2PlainBinary(keyText.mid(16, 16), sipherText1);
    QString sipherText3 = plain2SipherBinary(keyText.mid(0, 16), sipherText2);
    ui->lineEdit_tripleSipher->setText(sipherText3);
    //显示
}

void MainWindow::on_pushButton_tripleSipher2Plain_clicked()
{
    QString keyText = ui->lineEdit_tripleKey->text();
    QString sipherText = ui->lineEdit_tripleSipher->text();

    if(sipherText.length() != 16 || keyText.length() != 32)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    QRegularExpression regex("^[01]*$");
    bool isBinary = (regex.match(keyText).hasMatch()) && (regex.match(sipherText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "明文或者密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QString plainText1 = sipher2PlainBinary(keyText.mid(0, 16), sipherText);
    QString plainText2 = plain2SipherBinary(keyText.mid(16, 16), plainText1);
    QString plainText3 = sipher2PlainBinary(keyText.mid(0, 16), plainText2);
    ui->lineEdit_triplePlain->setText(plainText3);
    //显示
}


void MainWindow::on_pushButton_CBCPlain2Sipher_clicked()
{
    QString keyText = ui->lineEdit_CBCKey->text();
    QString plainText = ui->lineEdit_CBCPlain->text();
    //获取密钥和明文

    if(keyText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    if(plainText.length() % 2 != 0)
    {
        QMessageBox::information(nullptr, "提示", "明文长度不是双数", QMessageBox::Ok);
        return;
    }
    //明文长度不符合警告

    QRegularExpression regex_binary("^[01]*$");
    bool isBinary = regex_binary.match(keyText).hasMatch();
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QRegularExpression regex_ascii("^[\\x00-\\xFF]*$");
    bool isAscii = regex_ascii.match(plainText).hasMatch();
    if(!isAscii)
    {
        QMessageBox::information(nullptr, "提示", "密文不在ascii码表中", QMessageBox::Ok);
        return;
    }
    //非ascii码警告

    int randomIVInt = QRandomGenerator::global()->bounded(65536);
    QString IVText;
    QTextStream stream(&IVText);
    for(int i = 0; i < 16; i++)
    {
        if(randomIVInt >= qRound(qPow(2, 15-i)))
        {
            randomIVInt -= qRound(qPow(2, 15-i));
            stream << '1';
        }
        else
        {
            stream << '0';
        }
    }
    stream.flush();
    ui->lineEdit_CBCIV->setText(IVText);
    //加密随机生成IV

    QString sipherText = plain2SipherCBC(keyText, plainText, IVText);
    ui->lineEdit_CBCSipher->setText(sipherText);
    //显示
}


void MainWindow::on_pushButton_CBCSipher2Plain_clicked()
{
    QString keyText = ui->lineEdit_CBCKey->text();
    QString sipherText = ui->lineEdit_CBCSipher->text();
    QString IVText = ui->lineEdit_CBCIV->text();
    //获取密钥和明文和公钥

    if(keyText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "密钥长度不匹配", QMessageBox::Ok);
        return;
    }
    //长度不符合警告

    if(sipherText.length() % 2 != 0)
    {
        QMessageBox::information(nullptr, "提示", "明文长度不是双数", QMessageBox::Ok);
        return;
    }
    //明文长度不符合警告

    if(IVText.length() != 16)
    {
        QMessageBox::information(nullptr, "提示", "公钥长度出错", QMessageBox::Ok);
        return;
    }
    //公钥长度不符合警告

    QRegularExpression regex_binary("^[01]*$");
    bool isBinary = (regex_binary.match(keyText).hasMatch()) && (regex_binary.match(IVText).hasMatch());
    if(!isBinary)
    {
        QMessageBox::information(nullptr, "提示", "密钥不是二进制表示格式", QMessageBox::Ok);
        return;
    }
    //非二进制表示格式警告

    QRegularExpression regex_ascii("^[\\x00-\\xFF]*$");
    bool isAscii = regex_ascii.match(sipherText).hasMatch();
    if(!isAscii)
    {
        QMessageBox::information(nullptr, "提示", "密文不在ascii码表中", QMessageBox::Ok);
        return;
    }
    //非ascii码警告

    QString plainText = sipher2PlainCBC(keyText, sipherText, IVText);
    ui->lineEdit_CBCPlain->setText(plainText);
    //显示
}

