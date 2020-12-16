#include "systemstatus.h"
#include <QDebug>

SystemStatus::SystemStatus(QObject *parent) : QObject(parent) {
}

quint8  SystemStatus::getStatusFlags                () { return statusFlags              ; }
quint16 SystemStatus::getErrorFlags                 () { return errorsA | (errorsB << 8) ; }

quint16 SystemStatus::getEncoder                    () { return encoder                  ; }

quint32 SystemStatus::getPermCameraTriggers         () { return permCameraTriggers       ; }
quint32 SystemStatus::getPermAnswers                () { return permAnswers              ; }
quint32 SystemStatus::getPermNoAnswerFails          () { return permNoAnswerFails        ; }
quint32 SystemStatus::getPermPassed01               () { return permPassed01             ; }
quint32 SystemStatus::getPermPassed02               () { return permPassed02             ; }
quint32 SystemStatus::getPermPassed03               () { return permPassed03             ; }
quint32 SystemStatus::getPermPassed04               () { return permPassed04             ; }
quint32 SystemStatus::getPermFails01                () { return permFails01              ; }
quint32 SystemStatus::getPermFails02                () { return permFails02              ; }
quint32 SystemStatus::getPermFails03                () { return permFails03              ; }
quint32 SystemStatus::getPermFails04                () { return permFails04              ; }
quint32 SystemStatus::getPermExternalRejections     () { return permExternalRejections   ; }
quint32 SystemStatus::getPermRejectionsAgree        () { return permRejectionsAgree      ; }

quint32 SystemStatus::getGlobalCameraTriggers       () { return globalCameraTriggers     ; }
quint32 SystemStatus::getGlobalAnswers              () { return globalAnswers            ; }
quint32 SystemStatus::getGlobalNoAnswerFails        () { return globalNoAnswerFails      ; }
quint32 SystemStatus::getGlobalPassed01             () { return globalPassed01           ; }
quint32 SystemStatus::getGlobalPassed02             () { return globalPassed02           ; }
quint32 SystemStatus::getGlobalPassed03             () { return globalPassed03           ; }
quint32 SystemStatus::getGlobalPassed04             () { return globalPassed04           ; }
quint32 SystemStatus::getGlobalFails01              () { return globalFails01            ; }
quint32 SystemStatus::getGlobalFails02              () { return globalFails02            ; }
quint32 SystemStatus::getGlobalFails03              () { return globalFails03            ; }
quint32 SystemStatus::getGlobalFails04              () { return globalFails04            ; }
quint32 SystemStatus::getGlobalExternalRejections   () { return globalExternalRejections ; }
quint32 SystemStatus::getGlobalRejectionsAgree      () { return globalRejectionsAgree    ; }

quint32 SystemStatus::getBatchCameraTriggers        () { return batchCameraTriggers      ; }
quint32 SystemStatus::getBatchAnswers               () { return batchAnswers             ; }
quint32 SystemStatus::getBatchNoAnswerFails         () { return batchNoAnswerFails       ; }
quint32 SystemStatus::getBatchPassed01              () { return batchPassed01            ; }
quint32 SystemStatus::getBatchPassed02              () { return batchPassed02            ; }
quint32 SystemStatus::getBatchPassed03              () { return batchPassed03            ; }
quint32 SystemStatus::getBatchPassed04              () { return batchPassed04            ; }
quint32 SystemStatus::getBatchFails01               () { return batchFails01             ; }
quint32 SystemStatus::getBatchFails02               () { return batchFails02             ; }
quint32 SystemStatus::getBatchFails03               () { return batchFails03             ; }
quint32 SystemStatus::getBatchFails04               () { return batchFails04             ; }
quint32 SystemStatus::getBatchExternalRejections    () { return batchExternalRejections  ; }
quint32 SystemStatus::getBatchRejectionsAgree       () { return batchRejectionsAgree     ; }

quint32 SystemStatus::getDailyCameraTriggers        () { return dailyCameraTriggers      ; }
quint32 SystemStatus::getDailyAnswers               () { return dailyAnswers             ; }
quint32 SystemStatus::getDailyNoAnswerFails         () { return dailyNoAnswerFails       ; }
quint32 SystemStatus::getDailyPassed01              () { return dailyPassed01            ; }
quint32 SystemStatus::getDailyPassed02              () { return dailyPassed02            ; }
quint32 SystemStatus::getDailyPassed03              () { return dailyPassed03            ; }
quint32 SystemStatus::getDailyPassed04              () { return dailyPassed04            ; }
quint32 SystemStatus::getDailyFails01               () { return dailyFails01             ; }
quint32 SystemStatus::getDailyFails02               () { return dailyFails02             ; }
quint32 SystemStatus::getDailyFails03               () { return dailyFails03             ; }
quint32 SystemStatus::getDailyFails04               () { return dailyFails04             ; }
quint32 SystemStatus::getDailyExternalRejections    () { return dailyExternalRejections  ; }
quint32 SystemStatus::getDailyRejectionsAgree       () { return dailyRejectionsAgree     ; }

