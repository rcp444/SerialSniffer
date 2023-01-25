#pragma once
#define KPrint(s, ...) DbgPrintEx(0, 0, s, __VA_ARGS__);
#define KPrintInfo(s, ...) DbgPrintEx(0, 0, "SMON:INFO:: " ## s, __VA_ARGS__);
#define KPrintInfoN(s, ...) DbgPrintEx(0, 0, "SMON:INFO:: " ## s##"\n", __VA_ARGS__);
#define KPrintErr(s, ...) DbgPrintEx(0, 0, "SMON:ERR:: "## s, __VA_ARGS__);
#define KPrintErrN(s, ...) DbgPrintEx(0, 0, "SMON:ERR:: "## s##"\n", __VA_ARGS__);
#define KPrintN(s, ...) DbgPrintEx(0, 0, "SMON::: "## s##"\n", __VA_ARGS__);
