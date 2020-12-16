#include <QDebug>
#include "job.h"

// Step.Job.1 - {6E0599A1-41F8-11D0-AA2E-0020AF63DBFE}

/**
 * @brief Job::Job
 * @param parent
 */
Job::Job(QObject *parent) : QObject(parent)
{
    m_jobStep = new QAxObject("{6E0599A1-41F8-11D0-AA2E-0020AF63DBFE}");

    // setup job Load exception handler
    connect(m_jobStep,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onJobException(int, const QString &, const QString &, const QString &))
            );
}

/**
 * @brief Job::~Job
 */
Job::~Job() {
    if (m_jobStep != NULL)
        delete m_jobStep;
}

/**
 * @brief Job::load
 * @param avpFileName
 */
void Job::load(QString avpFileName) {
    m_jobStep->dynamicCall("Load([in] BSTR fileName)", avpFileName.toLatin1().data());
}

/**
 * @brief Job::removeAllJobs
 */
void Job::removeAllJobs() {
    qDebug() << "> CleanUp: removing all jobs from memory";
    while (m_jobStep->dynamicCall("Count").toInt() > 0) {
        qDebug() << "> - removing job";
        m_jobStep->dynamicCall("Remove(1)");
    }
}

/**
 * @brief Job::getSystemStep
 * @return
 */
QAxObject *Job::getSystemStep() {
    if (m_jobStep != NULL) {
        QAxObject *systemStep = m_jobStep->querySubObject("Item(1)");
        return systemStep;
    }
    return NULL;
}

/*!
 * \brief Job::getJobStep
 * \return
 */
QAxObject *Job::getJobStep() {
    return m_jobStep;
}

/**
 * @brief Job::onJobException
 * @param code
 * @param source
 * @param desc
 * @param help
 */
void Job::onJobException(int code, const QString &source, const QString &desc, const QString &help) {
    Q_UNUSED(code);
    Q_UNUSED(source);
    Q_UNUSED(desc);
    Q_UNUSED(help);

    // qCritical() << "Error on Step / Code:" << QString::number(code, 16) << " / Source:" << source << " / Description:" << desc << " / Help:" << help;
}

/**
 * @brief Job::walkStep
 * @param step
 * @param level
 */
void Job::walkStep(QAxObject* step, QList<StepProps> *stepList, StepProps parentProps, QString path, int level, walkCommand cmd) {
    // TODO: check if it's safe to remove path argument
    Q_UNUSED(path);

    QString type, symName;
    bool isTrainable, trained;
    StepProps stepProps;

    if (step == NULL || step->isNull())
        return;

    connect(step,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onJobException(int, const QString &, const QString &, const QString &))
            );

    type = step->property("Type").toString();

    // check if Step is trainable ...............................................................
    if (stepTypes.contains(type)) {
        isTrainable = stepTypes[type].trainable;
    }
    else {
        isTrainable = step->property("Trainable").toBool();
    }

    // check for command and act accordingly ....................................................
    switch (cmd) {
    case scanForSteps:
        stepProps = StepProperties::fromStepObject(step, parentProps);

        if (stepTypes.contains(type)) {
            stepProps.editRoi = stepTypes[type].editRoi;
        }

        if ( isTrainable || stepProps.editRoi || stepProps.datums.length() > 0) {
            stepList->append(stepProps);
        }
        break;
    case trainSteps:
        if (isTrainable) {
            trained = step->property("Trained").toBool();

            if (trained == false) {
                bool result = step->dynamicCall("Train()").toBool();
                qDebug() << (result ? "Succesful training of " : "Failed to train ") + symName;
            }
        }
        break;
    default:
        break;
    }

    int childrenCount = step->property("Count").toInt();
    for (int i = 0; i < childrenCount; i++) {
        QAxObject *child = step->querySubObject("Item([in] VARIANT varIndex)", i + 1);

        if (child != NULL && ! child->isNull()) {
            walkStep(child, stepList, stepProps, stepProps.path, level + 1, cmd);
            delete child;
        }
    }

    disconnect(step,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onJobException(int, const QString &, const QString &, const QString &))
            );
}

/*!
 * \brief Job::train
 * \param stepPath
 */
void Job::train(QString stepPath) {

    QAxObject *rootStep = getSystemStep();

    QString cmd = "Find(\"" + stepPath + "\", 2)";
    QAxObject *step = rootStep->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {

        bool result = step->dynamicCall("Train()").toBool();
        qDebug() << (result ? "Succesful training of " : "Failed to train ") + stepPath;

        delete step;
    }

    delete rootStep;
}

/*!
 * \brief Job::trainParent
 * \param stepPath
 */
void Job::trainParent(QString stepPath) {

    QAxObject *rootStep = getSystemStep();

    QString cmd = "Find(\"" + stepPath + "\", 2)";
    QAxObject *step = rootStep->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {

        QAxObject *parentStep = step->querySubObject("Parent");
        if (parentStep != NULL && ! parentStep->isNull()) {

            bool result = parentStep->dynamicCall("Train()").toBool();
            qDebug() << (result ? "Succesful training parent of " : "Failed to train parent of ") + stepPath;

            delete parentStep;
        }

        delete step;
    }

    delete rootStep;
}

/*!
 * \brief Job::deleteStep
 * \param stepPath
 */
void Job::deleteStep(QAxObject *step) {
    if (step == NULL || step->isNull()) {
        qCritical() << "Unable to delete invalid step";
        return;
    }

    QAxObject *parent = step->querySubObject("Parent");
    if (parent != NULL && ! parent->isNull()) {

        int childIndex = step->property("Index").toInt();
        parent->dynamicCall("RemoveStep([in] long Index, [in, optional, defaultvalue(1)] long delChildStep)", childIndex);

        delete parent;
    }
    else {
        qCritical() << "Unable to get parent of step " << step->property("Name").toString();
    }
}

/*!
 * \brief Job::setDatumValue
 * \param step
 * \param stepDatum
 */
void Job::setDatumValue(QAxObject *step, StepDatum stepDatum) {

    if (step == NULL || step->isNull()) {
        qCritical() << "Invalid step trying to set Datum value / " << stepDatum.symName;
        return;
    }

    if (stepDatum.type == tolPerc) {
        QStringList datumNames = stepDatum.symName.split(':');

        if (datumNames.size() < 3) {
            qCritical() << "Invalid tolerance datum name for step " + step->property("Name").toString();
        }
        else {
            setDatumRange(step, datumNames[0], datumNames[1], datumNames[2], stepDatum.value);
        }
    }
    else {
        // get step Datum
        QString cmd = "Datum(\"" +  stepDatum.symName + "\")";
        QAxObject *datum = step->querySubObject(cmd.toLatin1().data());

        if (datum != NULL && ! datum->isNull()) {

            // set the value
            datum->setProperty("Value", stepDatum.value / stepDatum.scale);
            step->dynamicCall("ApplyChanges()");
            // datum->dynamicCall("Regen(0)");
            // step->dynamicCall("RunWithPreRun", 0);

            qDebug() << "New datum value. Step:" << step->property("Name").toString() << "Value:" << stepDatum.value / stepDatum.scale;

            delete datum;
        }
    }
}

/*!
 * \brief Job::setDatumValue
 * \param stepPath
 * \param stepDatum
 */
void Job::setDatumValue(QString stepPath, StepDatum stepDatum) {

    QAxObject *rootStep;
    QAxObject *step;
    QString cmd;

    rootStep = getSystemStep();
    cmd = "Find(\"" + stepPath + "\", 2)";
    step = rootStep->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {

        if (stepDatum.childDatum) {

            QAxObject *avpCollection = step->querySubObject(QString("FindByType(\"" + stepDatum.childType + "\")").toLatin1().data());

            if (avpCollection != NULL && ! avpCollection->isNull()) {
                int count = avpCollection->property("Count").toInt();
                qDebug() << "Number of children:" << count;

                for (int i = 0; i < count; i++) {
                    QAxObject *child = step->querySubObject("Item([in] VARIANT varIndex)", i + 1);

                    if (child != NULL && ! child->isNull()) {

                        qDebug() << child->property("Name").toString();

                        setDatumValue(child, stepDatum);

                        delete child;
                    }
                }

                delete avpCollection;
            }
        }
        else {
            setDatumValue(step, stepDatum);
        }
        delete step;
    }
    delete rootStep;
    return;
}

/*!
 * \brief Job::setDatumRange
 * \param refDatum
 * \param minValueDatum
 * \param maxValueDatum
 * \param perc
 */
void Job::setDatumRange(QAxObject *step, QString refDatum, QString minValueDatum, QString maxValueDatum, int perc) {

    QString cmd;

    if (step != NULL && ! step->isNull()) {

        // get reference Datum
        cmd = "Datum(\"" +  refDatum + "\")";
        QAxObject *refDatum = step->querySubObject(cmd.toLatin1().data());

        // get min value Datum
        cmd = "Datum(\"" +  minValueDatum + "\")";
        QAxObject *minValueDatum = step->querySubObject(cmd.toLatin1().data());

        // get max value Datum
        cmd = "Datum(\"" +  maxValueDatum + "\")";
        QAxObject *maxValueDatum = step->querySubObject(cmd.toLatin1().data());

        if ( (refDatum != NULL && ! refDatum->isNull()) &&
             (minValueDatum != NULL && ! minValueDatum->isNull()) &&
             (maxValueDatum != NULL && ! maxValueDatum->isNull()) ) {

            refDatum->dynamicCall("Regen(0)");
            int refValue = refDatum->property("Value").toInt();
            int minValue = refValue * (1.0 - perc/100.0) + 0.5;
            int maxValue = refValue * (1.0 + perc/100.0) + 0.5;

            minValueDatum->setProperty("Value", minValue);
            maxValueDatum->setProperty("Value", maxValue);

            step->dynamicCall("ApplyChanges()");

            // minValueDatum->dynamicCall("Regen(0)");
            // maxValueDatum->dynamicCall("Regen(0)");

            qDebug() << "New datum values. Step:" << step->property("Name").toString() << "min value:" << minValue << "max value:" << maxValue;
        }

        if (refDatum != NULL) delete refDatum;
        if (minValueDatum != NULL) delete minValueDatum;
        if (maxValueDatum != NULL) delete maxValueDatum;
    }
}

/*!
 * \brief Job::regenerate
 */
void Job::regenerate() {
    QAxObject *rootStep = getSystemStep();

    QAxObject *inspection = rootStep->querySubObject("Find(\"Step.Inspection\", 0)");
    if (inspection != NULL && ! inspection->isNull()) {
        QAxObject *nothing = new QAxObject();

        inspection->dynamicCall("RunWithPreRun([in] IDatum *pChangedDatum)", QVariant::fromValue(nothing));
        delete inspection;
    }
}

/*!
 * \brief Job::fromBufferDm
 * \param bufferDm
 * \param withGraphics
 * \return
 */
QPixmap Job::fromBufferDm(QAxObject *bufferDm, bool withGraphics) {

    QPixmap pixmap;

    QAxObject *pictureDisp = bufferDm->querySubObject(
                "BufferAsPicture([in, optional, defaultvalue(1)] long withGraphics)",
                withGraphics ? 1 : 0
                );

    if (pictureDisp != NULL && ! pictureDisp->isNull()) {
        int hPic   = pictureDisp->property("Handle").toInt();
        int width  = pictureDisp->property("Width").toInt();
        int height = pictureDisp->property("Height").toInt();

        if (hPic && width > 0 && height > 0) {
            pixmap = QtWin::fromHBITMAP((HBITMAP)hPic);

            if (pixmap.isNull()) {
                qWarning() << "! Failed to create pixmap";
            }
        }
        else {
            qCritical() << "! Invalid picture handle";
        }
        delete pictureDisp;
    }

    return pixmap;
}

/*!
 * \brief Job::walkJob
 * \param stepList
 * \param cmd
 */
void Job::walkJob(QList<StepProps> *stepList, walkCommand cmd) {
    StepProps root;
    root.level = 0;
    QAxObject *rootStep = getSystemStep();


    // test test test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    QAxObject *thrsColorStep = rootStep->querySubObject("Find(\"Step.ThreshColor.1\", 0)");
//    if (thrsColorStep != NULL && ! thrsColorStep->isNull()) {
//        qDebug() << "Step symbolic name:" << thrsColorStep->property("NameSym").toString();

//        QAxObject *datumColor1Lower;
//        datumColor1Lower = thrsColorStep->querySubObject("Datum(\"Color1Lower\")");
//        if (datumColor1Lower != NULL && ! datumColor1Lower->isNull()) {
//            qDebug() << "Color1Lower Datum Value:" << datumColor1Lower->property("Value").toInt();
//        }

//        datumColor1Lower = thrsColorStep->querySubObject("Datum(\"Red Lower Threshold\")");
//        if (datumColor1Lower != NULL && ! datumColor1Lower->isNull()) {
//            qDebug() << "Red Lower Threshold Datum Value:" << datumColor1Lower->property("Value").toInt();
//        }

//        datumColor1Lower = thrsColorStep->querySubObject("Datum(\"Red  Upper Threshold\")");
//        if (datumColor1Lower != NULL && ! datumColor1Lower->isNull()) {
//            qDebug() << "Red Upper Threshold Datum Value:" << datumColor1Lower->property("Value").toInt();
//        }

//        int count = thrsColorStep->querySubObject("Datums")->property("Count").toInt();
//        QAxObject* datums = thrsColorStep->querySubObject("Datums");
//        for (int i = 0; i < count; i ++) {
//            qDebug() << datums->querySubObject("Item([in] VARIANT varIndex)", i + 1)->property("NameSym").toString();
//        }

//    }

    // test test test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    walkStep(rootStep, stepList, root, "", 0, cmd);
    delete rootStep;
}

/*!
 * \brief Job::getAvpNumericParameter
 * \param stepSym
 * \param datumSym
 * \return
 */
QVariant Job::getAvpNumericParameter(int inspectionId, QString stepSym, QString datumSym) {

    QString cmd;
    QVariant variantValue = QVariant::fromValue(0);

    // find the step
    QAxObject *rootStep = getSystemStep();
    QAxObject *insp[5];
    insp[0] = getJobStep()->querySubObject("Find(\"Step.Inspection\", 0)");
    insp[1] = insp[0]->querySubObject("Next()");
    insp[2] = insp[1]->querySubObject("Next()");
    insp[3] = insp[2]->querySubObject("Next()");
    insp[4] = insp[3]->querySubObject("Next()");

    cmd = "Find(\"" + stepSym + "\", 0)";
    QAxObject *step = insp[inspectionId]->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {
        // get the Datum
        cmd = "Datum(\"" + datumSym + "\")";
        QAxObject *datum = step->querySubObject(cmd.toLatin1().data());

        if (datum != NULL && ! datum->isNull()) {
            variantValue = datum->property("Value");
            delete datum;
        }
        delete step;
    }
    delete rootStep;

    return variantValue;
}

/*!
 * \brief Job::setAvpNumericParam
 * \param code
 * \param value
 */
void Job::setAvpNumericParameter(int inspectionId, QString stepSym, QString datumSym, QVariant value) {
    QString cmd;

    // find the step
    QAxObject *rootStep = getSystemStep();
    QAxObject *insp[5];
    insp[0] = getJobStep()->querySubObject("Find(\"Step.Inspection\", 0)");
    insp[1] = insp[0]->querySubObject("Next()");
    insp[2] = insp[1]->querySubObject("Next()");
    insp[3] = insp[2]->querySubObject("Next()");
    insp[4] = insp[3]->querySubObject("Next()");

    cmd = "Find(\"" + stepSym + "\", 0)";
    QAxObject *step = insp[inspectionId]->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {
        // get the Datum
        cmd = "Datum(\"" + datumSym + "\")";
        QAxObject *datum = step->querySubObject(cmd.toLatin1().data());

        if (datum != NULL && ! datum->isNull()) {
            datum->setProperty("Value", value);

            delete datum;
        }

        delete step;
    }

    delete rootStep;
}

/*!
 * \brief Job::updateROI
 * \param stepProps
 */
void Job::updateROI(StepProps stepProps) {

    // find the step
    QAxObject *rootStep = getSystemStep();
    QAxObject *step = rootStep->querySubObject(QString("Find(\"" + stepProps.path + "\", 2)").toLatin1().data());

    if (step != NULL && ! step->isNull()) {

        QAxObject *stepRoi = step->querySubObject("Datum(\"ROI\")");

        if (stepRoi != NULL && ! stepRoi->isNull()) {

            QVariantList newROI;
            newROI.append(QVariant::fromValue(stepProps.roiValue[0] + stepProps.roiValue[2]/2.0));
            newROI.append(QVariant::fromValue(stepProps.roiValue[1] + stepProps.roiValue[3]/2.0));
            newROI.append(QVariant::fromValue(stepProps.roiValue[2]));
            newROI.append(QVariant::fromValue(stepProps.roiValue[3]));
            newROI.append(QVariant::fromValue(0));

            stepRoi->setProperty("Value", newROI);
            step->dynamicCall("ApplyChanges()");
            delete stepRoi;

//            if (stepProps.autoTrainParent) {
//                train(stepProps.parentPath);
//            }
        }
        delete step;
    }
    delete rootStep;
}

/*!
 * \brief Job::getStepROI
 * \param step
 * \return
 */
QRect Job::getStepROI(QAxObject *step) {

    QRect rect;

    QAxObject *roi = step->querySubObject("Datum(\"ROI\")");

    if (roi != NULL && !roi->isNull()) {
        QList<QVariant> roiVar = roi->property("Value").toList();

        if (roiVar.length() >= 4) {
            rect = QRect(
                        roiVar.at(0).toInt() - roiVar.at(2).toInt()/2,
                        roiVar.at(1).toInt() - roiVar.at(3).toInt()/2,
                        roiVar.at(2).toInt(),
                        roiVar.at(3).toInt()
                        );
        }
        delete roi;
    }
    return rect;
}

/*!
 * \brief Job::getStepFullPath
 * \param root the root step is used as the base for searching for the target step
 * \param stepName the (user)name of the step to get the full path
 * \return
 */
QString Job::getStepFullPath(QAxObject *root, QString stepName) {

    QString path = "";

    if (root == NULL || root->isNull())
        return path;

    // FIND_BY_USERNAME: 1
    QAxObject *step = root->querySubObject(QString("Find(\"" + stepName + "\", 1)").toLatin1().data());
    if (step != NULL && ! step->isNull()) {
        path = getFullPath(step, "");
        delete step;
        return path;
    }

    return path;
}

/*!
 * \brief Job::getStepFullPath
 * \param step
 * \param startPath
 * \return
 */
QString Job::getFullPath(QAxObject *step, QString belowPath) {

    QString newPath;

    if (step == NULL || step->isNull()) {
        return belowPath;
    }

    QAxObject *parentStep = step->querySubObject("Parent");
    if (parentStep != NULL && ! parentStep->isNull()) {

        QString stepSymName = step->property("NameSym").toString();
        if (!belowPath.isEmpty()) {
            newPath = stepSymName + "." + belowPath;
        }
        else {
            newPath = stepSymName;
        }

        QString stepType = step->property("Type").toString();
        if (stepType.compare("Step.SnapShot.1", Qt::CaseInsensitive) != 0) {
            newPath = getFullPath(parentStep, newPath);
        }

        delete parentStep;
        return newPath;
    }

    return belowPath;
}
