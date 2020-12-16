#ifndef SYSTEMSTATUS_H
#define SYSTEMSTATUS_H

#include <QObject>

#define SYSTEM_STARTED_FLAG         0x01
#define SYSTEM_STOPED_FLAG          0x02
#define SYSTEM_WARNING_FLAG         0x04
#define SYSTEM_ERROR_FLAG           0x08
#define SYSTEM_MANUAL_FLAG          0x10
#define SYSTEM_CONTINUEONERROR_FLAG 0x20
#define SYSTEM_STROBEALWAYSON_FLAG  0x40

#define SYSTEM_ERROR_A_CONSECUTIVE_NOANSWERS    0x01
#define SYSTEM_ERROR_A_CONSECUTIVE_FAILES       0x02
#define SYSTEM_ERROR_A_BLISTER_DOUBLEID         0x04
#define SYSTEM_ERROR_A_BUFFER_FULL              0x08
#define SYSTEM_ERROR_A_RESERVED                 0x10
#define SYSTEM_ERROR_A_STEP1_ERROR              0x20
#define SYSTEM_ERROR_A_STEP2_ERROR              0x40
#define SYSTEM_ERROR_A_RESERVED2                0x80

#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER1_FAILES  0x0100
#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER2_FAILES  0x0200
#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER3_FAILES  0x0400
#define SYSTEM_ERROR_A_CONSECUTIVE_BLISTER4_FAILES  0x0800


class SystemStatus: public QObject {
    Q_OBJECT

    Q_PROPERTY(quint8  statusFlags               READ getStatusFlags)
    Q_PROPERTY(quint16 errorFlags                READ getErrorFlags)
    Q_PROPERTY(quint16 encoder                   READ getEncoder)

    Q_PROPERTY(quint32 permCameraTriggers        READ getPermCameraTriggers       )
    Q_PROPERTY(quint32 permAnswers               READ getPermAnswers              )
    Q_PROPERTY(quint32 permNoAnswerFails         READ getPermNoAnswerFails        )
    Q_PROPERTY(quint32 permPassed01              READ getPermPassed01             )
    Q_PROPERTY(quint32 permPassed02              READ getPermPassed02             )
    Q_PROPERTY(quint32 permPassed03              READ getPermPassed03             )
    Q_PROPERTY(quint32 permPassed04              READ getPermPassed04             )
    Q_PROPERTY(quint32 permFails01               READ getPermFails01              )
    Q_PROPERTY(quint32 permFails02               READ getPermFails02              )
    Q_PROPERTY(quint32 permFails03               READ getPermFails03              )
    Q_PROPERTY(quint32 permFails04               READ getPermFails04              )
    Q_PROPERTY(quint32 permExternalRejections    READ getPermExternalRejections   )
    Q_PROPERTY(quint32 permRejectionsAgree       READ getPermRejectionsAgree      )

    Q_PROPERTY(quint32 globalCameraTriggers      READ getGlobalCameraTriggers     )
    Q_PROPERTY(quint32 globalAnswers             READ getGlobalAnswers            )
    Q_PROPERTY(quint32 globalNoAnswerFails       READ getGlobalNoAnswerFails      )
    Q_PROPERTY(quint32 globalPassed01            READ getGlobalPassed01           )
    Q_PROPERTY(quint32 globalPassed02            READ getGlobalPassed02           )
    Q_PROPERTY(quint32 globalPassed03            READ getGlobalPassed03           )
    Q_PROPERTY(quint32 globalPassed04            READ getGlobalPassed04           )
    Q_PROPERTY(quint32 globalFails01             READ getGlobalFails01            )
    Q_PROPERTY(quint32 globalFails02             READ getGlobalFails02            )
    Q_PROPERTY(quint32 globalFails03             READ getGlobalFails03            )
    Q_PROPERTY(quint32 globalFails04             READ getGlobalFails04            )
    Q_PROPERTY(quint32 globalExternalRejections  READ getGlobalExternalRejections )
    Q_PROPERTY(quint32 globalRejectionsAgree     READ getGlobalRejectionsAgree    )

    Q_PROPERTY(quint32 batchCameraTriggers       READ getBatchCameraTriggers      )
    Q_PROPERTY(quint32 batchAnswers              READ getBatchAnswers             )
    Q_PROPERTY(quint32 batchNoAnswerFails        READ getBatchNoAnswerFails       )
    Q_PROPERTY(quint32 batchPassed01             READ getBatchPassed01            )
    Q_PROPERTY(quint32 batchPassed02             READ getBatchPassed02            )
    Q_PROPERTY(quint32 batchPassed03             READ getBatchPassed03            )
    Q_PROPERTY(quint32 batchPassed04             READ getBatchPassed04            )
    Q_PROPERTY(quint32 batchFails01              READ getBatchFails01             )
    Q_PROPERTY(quint32 batchFails02              READ getBatchFails02             )
    Q_PROPERTY(quint32 batchFails03              READ getBatchFails03             )
    Q_PROPERTY(quint32 batchFails04              READ getBatchFails04             )
    Q_PROPERTY(quint32 batchExternalRejections   READ getBatchExternalRejections  )
    Q_PROPERTY(quint32 batchRejectionsAgree      READ getBatchRejectionsAgree     )

    Q_PROPERTY(quint32 dailyCameraTriggers       READ getDailyCameraTriggers      )
    Q_PROPERTY(quint32 dailyAnswers              READ getDailyAnswers             )
    Q_PROPERTY(quint32 dailyNoAnswerFails        READ getDailyNoAnswerFails       )
    Q_PROPERTY(quint32 dailyPassed01             READ getDailyPassed01            )
    Q_PROPERTY(quint32 dailyPassed02             READ getDailyPassed02            )
    Q_PROPERTY(quint32 dailyPassed03             READ getDailyPassed03            )
    Q_PROPERTY(quint32 dailyPassed04             READ getDailyPassed04            )
    Q_PROPERTY(quint32 dailyFails01              READ getDailyFails01             )
    Q_PROPERTY(quint32 dailyFails02              READ getDailyFails02             )
    Q_PROPERTY(quint32 dailyFails03              READ getDailyFails03             )
    Q_PROPERTY(quint32 dailyFails04              READ getDailyFails04             )
    Q_PROPERTY(quint32 dailyExternalRejections   READ getDailyExternalRejections  )
    Q_PROPERTY(quint32 dailyRejectionsAgree      READ getDailyRejectionsAgree     )

public:
    explicit SystemStatus(QObject *parent = 0);

    quint8  getStatusFlags();
    quint16 getErrorFlags();

    quint16 getEncoder();

    quint32 getPermCameraTriggers       ();
    quint32 getPermAnswers              ();
    quint32 getPermNoAnswerFails        ();
    quint32 getPermPassed01             ();
    quint32 getPermPassed02             ();
    quint32 getPermPassed03             ();
    quint32 getPermPassed04             ();
    quint32 getPermFails01              ();
    quint32 getPermFails02              ();
    quint32 getPermFails03              ();
    quint32 getPermFails04              ();
    quint32 getPermExternalRejections   ();
    quint32 getPermRejectionsAgree      ();

    quint32 getGlobalCameraTriggers     ();
    quint32 getGlobalAnswers            ();
    quint32 getGlobalNoAnswerFails      ();
    quint32 getGlobalPassed01           ();
    quint32 getGlobalPassed02           ();
    quint32 getGlobalPassed03           ();
    quint32 getGlobalPassed04           ();
    quint32 getGlobalFails01            ();
    quint32 getGlobalFails02            ();
    quint32 getGlobalFails03            ();
    quint32 getGlobalFails04            ();
    quint32 getGlobalExternalRejections ();
    quint32 getGlobalRejectionsAgree    ();

    quint32 getBatchCameraTriggers      ();
    quint32 getBatchAnswers             ();
    quint32 getBatchNoAnswerFails       ();
    quint32 getBatchPassed01            ();
    quint32 getBatchPassed02            ();
    quint32 getBatchPassed03            ();
    quint32 getBatchPassed04            ();
    quint32 getBatchFails01             ();
    quint32 getBatchFails02             ();
    quint32 getBatchFails03             ();
    quint32 getBatchFails04             ();
    quint32 getBatchExternalRejections  ();
    quint32 getBatchRejectionsAgree     ();

    quint32 getDailyCameraTriggers      ();
    quint32 getDailyAnswers             ();
    quint32 getDailyNoAnswerFails       ();
    quint32 getDailyPassed01            ();
    quint32 getDailyPassed02            ();
    quint32 getDailyPassed03            ();
    quint32 getDailyPassed04            ();
    quint32 getDailyFails01             ();
    quint32 getDailyFails02             ();
    quint32 getDailyFails03             ();
    quint32 getDailyFails04             ();
    quint32 getDailyExternalRejections  ();
    quint32 getDailyRejectionsAgree     ();

    quint8 statusFlags;
    quint8 reservedA;

    quint32 permCameraTriggers;
    quint32 permAnswers;
    quint32 permNoAnswerFails;
    quint32 permPassed01;
    quint32 permPassed02;
    quint32 permPassed03;
    quint32 permPassed04;
    quint32 permFails01;
    quint32 permFails02;
    quint32 permFails03;
    quint32 permFails04;
    quint32 permExternalRejections;
    quint32 permRejectionsAgree;

    quint32 globalCameraTriggers;
    quint32 globalAnswers;
    quint32 globalNoAnswerFails;
    quint32 globalPassed01;
    quint32 globalPassed02;
    quint32 globalPassed03;
    quint32 globalPassed04;
    quint32 globalFails01;
    quint32 globalFails02;
    quint32 globalFails03;
    quint32 globalFails04;
    quint32 globalExternalRejections;
    quint32 globalRejectionsAgree;

    quint32 batchCameraTriggers;
    quint32 batchAnswers;
    quint32 batchNoAnswerFails;
    quint32 batchPassed01;
    quint32 batchPassed02;
    quint32 batchPassed03;
    quint32 batchPassed04;
    quint32 batchFails01;
    quint32 batchFails02;
    quint32 batchFails03;
    quint32 batchFails04;
    quint32 batchExternalRejections;
    quint32 batchRejectionsAgree;

    quint32 dailyCameraTriggers;
    quint32 dailyAnswers;
    quint32 dailyNoAnswerFails;
    quint32 dailyPassed01;
    quint32 dailyPassed02;
    quint32 dailyPassed03;
    quint32 dailyPassed04;
    quint32 dailyFails01;
    quint32 dailyFails02;
    quint32 dailyFails03;
    quint32 dailyFails04;
    quint32 dailyExternalRejections;
    quint32 dailyRejectionsAgree;

    quint16 encoder;
    quint8 warningsA;
    quint8 warningsB;

    quint8 errorsA;
    quint8 errorsB;
    quint8 errorsC;
    quint8 errorsD;
};

#endif // SYSTEMSTATUS_H
