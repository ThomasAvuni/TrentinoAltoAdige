// DebugMacros.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

// Helper per convertire qualsiasi tipo in stringa
template<typename T>
FString ToDebugString(const T& Value)
{
    if constexpr (std::is_same_v<T, FString>)
        return Value;
    else if constexpr (std::is_same_v<T, FText>)
        return Value.ToString();
    else if constexpr (std::is_same_v<T, FName>)
        return Value.ToString();
    else if constexpr (std::is_same_v<T, bool>)
        return Value ? TEXT("True") : TEXT("False");
    else if constexpr (std::is_same_v<T, FVector>)
        return FString::Printf(TEXT("(%.2f, %.2f, %.2f)"), Value.X, Value.Y, Value.Z);
    else if constexpr (std::is_same_v<T, FVector2D>)
        return FString::Printf(TEXT("(%.2f, %.2f)"), Value.X, Value.Y);
    else if constexpr (std::is_same_v<T, FRotator>)
        return FString::Printf(TEXT("(%.2f, %.2f, %.2f)"), Value.Pitch, Value.Yaw, Value.Roll);
    else if constexpr (std::is_same_v<T, FTransform>)
        return FString::Printf(TEXT("Loc:(%.2f,%.2f,%.2f) Rot:(%.2f,%.2f,%.2f)"), 
            Value.GetLocation().X, Value.GetLocation().Y, Value.GetLocation().Z,
            Value.GetRotation().Euler().X, Value.GetRotation().Euler().Y, Value.GetRotation().Euler().Z);
    else if constexpr (std::is_integral_v<T>)
        return FString::Printf(TEXT("%d"), static_cast<int32>(Value));
    else if constexpr (std::is_floating_point_v<T>)
        return FString::Printf(TEXT("%.2f"), static_cast<float>(Value));
    else if constexpr (std::is_pointer_v<T>)
    {
        if (Value == nullptr)
            return TEXT("nullptr");
        else
        {
            // Prova a convertire oggetti UE se possibile
            if constexpr (std::is_base_of_v<UObject, std::remove_pointer_t<T>>)
            {
                if (IsValid(Value))
                    return Value->GetName(); // Usa GetName() direttamente senza *
                else
                    return TEXT("Invalid Object");
            }
            else
                return FString::Printf(TEXT("0x%p"), Value);
        }
    }
    else
        return TEXT("Unknown Type");
}

// Funzione ricorsiva per sostituire {} con i valori
template<typename T, typename... Args>
FString FormatDebugString(const FString& Format, const T& First, Args&&... Rest)
{
    FString Result = Format;
    int32 Index = Result.Find(TEXT("{}"));
    if (Index != INDEX_NONE)
    {
        FString ValueStr = ToDebugString(First);
        Result = Result.Mid(0, Index) + ValueStr + Result.Mid(Index + 2);
        
        if constexpr (sizeof...(Rest) > 0)
            return FormatDebugString(Result, Rest...);
    }
    return Result;
}

// Specializzazione per quando non ci sono più argomenti
inline FString FormatDebugString(const FString& Format)
{
    return Format;
}

// Ottieni automaticamente nome classe e funzione
#define GET_CLASS_FUNC() FString::Printf(TEXT("[%s::%s]"), \
    *GetClass()->GetName(), \
    TEXT(__FUNCTION__))

// Debug universale - supporta {} per qualsiasi tipo E messaggi semplici senza parametri
#define DBG(...) \
    if (GEngine) \
    { \
        FString Msg; \
        if constexpr (sizeof(__VA_ARGS__) == 1) \
        { \
            /* Solo il formato, nessun parametro */ \
            Msg = FString(FIRST_ARG(__VA_ARGS__)); \
        } \
        else if constexpr (sizeof(__VA_ARGS__) > 1) \
        { \
            /* Formato + parametri */ \
            Msg = FormatDebugString(FString(FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__)); \
        } \
        int32 Key = GetTypeHash(GetClass()->GetName()); \
        GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::Green, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Helper macros per gestire i parametri variabili
#define FIRST_ARG(first, ...) first
#define REST_ARGS(first, ...) __VA_ARGS__

// Versione più semplice che funziona sempre
/*
#define DBG(...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()); \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Green, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }
*/
// Funzione helper per processare gli argomenti
template<typename... Args>
FString ProcessDebugArgs(const FString& Format, Args&&... Arguments)
{
    if constexpr (sizeof...(Arguments) > 0)
        return FormatDebugString(Format, Arguments...);
    else
        return Format;
}

// Specializzazione per solo il formato
inline FString ProcessDebugArgs(const FString& Format)
{
    return Format;
}

// Debug con colore
#define DBG_COLOR(Color, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()); \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, Color, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Warning (giallo)
#define DBG_WARN(...) \
    DBG_COLOR(FColor::Yellow, __VA_ARGS__)

// Warning con key personalizzata
#define DBG_WARN_KEY(CustomKey, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()) + CustomKey; \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Yellow, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Errore (rosso)  
#define DBG_ERROR(...) \
    DBG_COLOR(FColor::Red, __VA_ARGS__)

// Error con key personalizzata
#define DBG_ERROR_KEY(CustomKey, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()) + CustomKey; \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Red, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Debug con durata personalizzata
#define DBG_TIMED(Duration, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()); \
        GEngine->AddOnScreenDebugMessage(Key, Duration, FColor::Green, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// ⭐ DBG_LINE - Key automatica basata su numero di riga ⭐
#define DBG_LINE_DURATION(Duration, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()) + __LINE__; \
        GEngine->AddOnScreenDebugMessage(Key, Duration, FColor::Green, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

#define DBG_LINE(...) \
if (GEngine) \
{ \
FString Msg = ProcessDebugArgs(__VA_ARGS__); \
int32 Key = GetTypeHash(GetClass()->GetName()) + __LINE__; \
GEngine->AddOnScreenDebugMessage(Key, 2.f, FColor::Green, \
FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
}

// DBG_LINE con warning (giallo)
#define DBG_LINE_WARN(...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()) + __LINE__; \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Yellow, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// DBG_LINE con errore (rosso)
#define DBG_LINE_ERROR(...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()) + __LINE__; \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Red, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Debug con key personalizzata
#define DBG_KEY(CustomKey, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(GetClass()->GetName()) + CustomKey; \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Green, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Debug per cicli for
#define DBG_LOOP(Index, ...) \
    DBG_KEY(Index, __VA_ARGS__)

// Debug con key crescente automatica
#define DBG_UNIQUE(...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        static int32 UniqueCounter = 0; \
        int32 Key = GetTypeHash(GetClass()->GetName()) + (++UniqueCounter); \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Green, \
            FString::Printf(TEXT("%s %s"), *GET_CLASS_FUNC(), *Msg)); \
    }

// Debug condizionale
#define DBG_IF(Condition, ...) \
    if (Condition) { DBG(__VA_ARGS__); }

// Per funzioni statiche
#define DBG_STATIC(ClassName, ...) \
    if (GEngine) \
    { \
        FString Msg = ProcessDebugArgs(__VA_ARGS__); \
        int32 Key = GetTypeHash(TEXT(#ClassName)); \
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Cyan, \
            FString::Printf(TEXT("[%s::%s] %s"), TEXT(#ClassName), TEXT(__FUNCTION__), *Msg)); \
    }
