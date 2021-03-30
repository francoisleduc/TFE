; ModuleID = 'xdp_prog_kern.c'
source_filename = "xdp_prog_kern.c"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "bpf"

%struct.bpf_map_def = type { i32, i32, i32, i32, i32 }
%struct.xdp_md = type { i32, i32, i32 }
%struct.ethhdr = type { [6 x i8], [6 x i8], i16 }

@xdp_stats_map = global %struct.bpf_map_def { i32 2, i32 4, i32 32, i32 1, i32 0 }, section "maps", align 4, !dbg !0
@xdp_stats1_func.____fmt = private unnamed_addr constant [31 x i8] c"payload_size %u - , proto: %u\0A\00", align 1
@xdp_stats1_func.____fmt.1 = private unnamed_addr constant [23 x i8] c"Matching bits SYN+ACK\0A\00", align 1
@_license = global [4 x i8] c"GPL\00", section "license", align 1, !dbg !48
@llvm.used = appending global [3 x i8*] [i8* getelementptr inbounds ([4 x i8], [4 x i8]* @_license, i32 0, i32 0), i8* bitcast (i32 (%struct.xdp_md*)* @xdp_stats1_func to i8*), i8* bitcast (%struct.bpf_map_def* @xdp_stats_map to i8*)], section "llvm.metadata"

; Function Attrs: nounwind
define i32 @xdp_stats1_func(%struct.xdp_md* nocapture readonly) #0 section "xdp_stats1" !dbg !85 {
  %2 = alloca i32, align 4
  %3 = alloca [31 x i8], align 1
  %4 = alloca [23 x i8], align 1
  call void @llvm.dbg.value(metadata %struct.xdp_md* %0, metadata !95, metadata !DIExpression()), !dbg !183
  %5 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 1, !dbg !184
  %6 = load i32, i32* %5, align 4, !dbg !184, !tbaa !185
  %7 = zext i32 %6 to i64, !dbg !190
  %8 = inttoptr i64 %7 to i8*, !dbg !191
  call void @llvm.dbg.value(metadata i8* %8, metadata !96, metadata !DIExpression()), !dbg !192
  %9 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 0, !dbg !193
  %10 = load i32, i32* %9, align 4, !dbg !193, !tbaa !194
  %11 = zext i32 %10 to i64, !dbg !195
  %12 = inttoptr i64 %11 to i8*, !dbg !196
  call void @llvm.dbg.value(metadata i8* %12, metadata !97, metadata !DIExpression()), !dbg !197
  %13 = bitcast i32* %2 to i8*, !dbg !198
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %13) #3, !dbg !198
  call void @llvm.dbg.value(metadata i32 0, metadata !98, metadata !DIExpression()), !dbg !199
  store i32 0, i32* %2, align 4, !dbg !199, !tbaa !200
  %14 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @xdp_stats_map to i8*), i8* nonnull %13) #3, !dbg !201
  call void @llvm.dbg.value(metadata i8* %14, metadata !99, metadata !DIExpression()), !dbg !202
  %15 = icmp eq i8* %14, null, !dbg !203
  br i1 %15, label %85, label %16, !dbg !205

; <label>:16:                                     ; preds = %1
  %17 = sub nsw i64 %7, %11, !dbg !206
  call void @llvm.dbg.value(metadata i64 %17, metadata !110, metadata !DIExpression()), !dbg !207
  %18 = bitcast i8* %14 to i64*, !dbg !208
  %19 = load i64, i64* %18, align 8, !dbg !209, !tbaa !210
  %20 = add i64 %19, 1, !dbg !209
  store i64 %20, i64* %18, align 8, !dbg !209, !tbaa !210
  %21 = getelementptr inbounds i8, i8* %14, i64 8, !dbg !213
  %22 = bitcast i8* %21 to i64*, !dbg !213
  %23 = load i64, i64* %22, align 8, !dbg !214, !tbaa !215
  %24 = add i64 %17, %23, !dbg !214
  store i64 %24, i64* %22, align 8, !dbg !214, !tbaa !215
  %25 = inttoptr i64 %11 to %struct.ethhdr*, !dbg !216
  call void @llvm.dbg.value(metadata %struct.ethhdr* %25, metadata !112, metadata !DIExpression()), !dbg !217
  %26 = getelementptr i8, i8* %12, i64 14, !dbg !218
  %27 = icmp ugt i8* %26, %8, !dbg !220
  %28 = getelementptr i8, i8* %12, i64 34, !dbg !221
  %29 = icmp ugt i8* %28, %8, !dbg !223
  %30 = or i1 %27, %29, !dbg !224
  call void @llvm.dbg.value(metadata i8* %26, metadata !129, metadata !DIExpression()), !dbg !225
  br i1 %30, label %85, label %31, !dbg !224

; <label>:31:                                     ; preds = %16
  %32 = getelementptr inbounds i8, i8* %12, i64 23, !dbg !226
  %33 = load i8, i8* %32, align 1, !dbg !226, !tbaa !228
  %34 = icmp eq i8 %33, 6, !dbg !231
  br i1 %34, label %35, label %85, !dbg !232

; <label>:35:                                     ; preds = %31
  %36 = getelementptr i8, i8* %12, i64 54, !dbg !233
  %37 = icmp ugt i8* %36, %8, !dbg !235
  br i1 %37, label %85, label %38, !dbg !236

; <label>:38:                                     ; preds = %35
  %39 = getelementptr inbounds i8, i8* %12, i64 16, !dbg !237
  %40 = bitcast i8* %39 to i16*, !dbg !237
  %41 = load i16, i16* %40, align 2, !dbg !237, !tbaa !238
  %42 = call i16 @llvm.bswap.i16(i16 %41), !dbg !237
  %43 = zext i16 %42 to i32, !dbg !237
  %44 = getelementptr inbounds i8, i8* %12, i64 46, !dbg !239
  %45 = bitcast i8* %44 to i16*, !dbg !239
  %46 = load i16, i16* %45, align 4, !dbg !239
  %47 = lshr i16 %46, 2, !dbg !240
  %48 = and i16 %47, 60, !dbg !240
  %49 = zext i16 %48 to i32, !dbg !240
  %50 = sub nsw i32 %43, %49, !dbg !241
  %51 = load i8, i8* %26, align 4, !dbg !242
  %52 = shl i8 %51, 2, !dbg !243
  %53 = and i8 %52, 60, !dbg !243
  %54 = zext i8 %53 to i32, !dbg !243
  %55 = sub nsw i32 %50, %54, !dbg !244
  call void @llvm.dbg.value(metadata i32 %55, metadata !111, metadata !DIExpression()), !dbg !245
  %56 = getelementptr inbounds [31 x i8], [31 x i8]* %3, i64 0, i64 0, !dbg !246
  call void @llvm.lifetime.start.p0i8(i64 31, i8* nonnull %56) #3, !dbg !246
  call void @llvm.dbg.declare(metadata [31 x i8]* %3, metadata !170, metadata !DIExpression()), !dbg !246
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %56, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @xdp_stats1_func.____fmt, i64 0, i64 0), i64 31, i32 1, i1 false), !dbg !246
  %57 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %25, i64 0, i32 2, !dbg !246
  %58 = load i16, i16* %57, align 1, !dbg !246, !tbaa !247
  %59 = call i16 @llvm.bswap.i16(i16 %58), !dbg !246
  %60 = zext i16 %59 to i32, !dbg !246
  %61 = call i32 (i8*, i32, ...) inttoptr (i64 6 to i32 (i8*, i32, ...)*)(i8* nonnull %56, i32 31, i32 %55, i32 %60) #3, !dbg !246
  call void @llvm.lifetime.end.p0i8(i64 31, i8* nonnull %56) #3, !dbg !249
  call void @llvm.dbg.value(metadata i8* %36, metadata !127, metadata !DIExpression()), !dbg !250
  %62 = zext i32 %55 to i64, !dbg !251
  %63 = getelementptr i8, i8* %36, i64 %62, !dbg !251
  %64 = icmp ugt i8* %63, %8, !dbg !253
  %65 = getelementptr i8, i8* %12, i64 48, !dbg !254
  %66 = icmp ugt i8* %65, %8, !dbg !256
  %67 = or i1 %66, %64, !dbg !257
  call void @llvm.dbg.value(metadata i8* %69, metadata !175, metadata !DIExpression()), !dbg !258
  br i1 %67, label %85, label %68, !dbg !257

; <label>:68:                                     ; preds = %38
  %69 = getelementptr inbounds i8, i8* %12, i64 47, !dbg !259
  %70 = load i8, i8* %69, align 1, !dbg !260, !tbaa !261
  %71 = and i8 %70, 18, !dbg !262
  %72 = icmp eq i8 %71, 18, !dbg !262
  br i1 %72, label %73, label %80, !dbg !262

; <label>:73:                                     ; preds = %68
  %74 = getelementptr inbounds i8, i8* %14, i64 16, !dbg !263
  %75 = bitcast i8* %74 to i64*, !dbg !263
  %76 = load i64, i64* %75, align 8, !dbg !263, !tbaa !264
  %77 = add i64 %76, 1, !dbg !265
  store i64 %77, i64* %75, align 8, !dbg !266, !tbaa !264
  %78 = getelementptr inbounds [23 x i8], [23 x i8]* %4, i64 0, i64 0, !dbg !267
  call void @llvm.lifetime.start.p0i8(i64 23, i8* nonnull %78) #3, !dbg !267
  call void @llvm.dbg.declare(metadata [23 x i8]* %4, metadata !176, metadata !DIExpression()), !dbg !267
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %78, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @xdp_stats1_func.____fmt.1, i64 0, i64 0), i64 23, i32 1, i1 false), !dbg !267
  %79 = call i32 (i8*, i32, ...) inttoptr (i64 6 to i32 (i8*, i32, ...)*)(i8* nonnull %78, i32 23) #3, !dbg !267
  call void @llvm.lifetime.end.p0i8(i64 23, i8* nonnull %78) #3, !dbg !268
  br label %80, !dbg !269

; <label>:80:                                     ; preds = %68, %73
  %81 = getelementptr inbounds i8, i8* %14, i64 24, !dbg !270
  %82 = bitcast i8* %81 to i64*, !dbg !270
  %83 = load i64, i64* %82, align 8, !dbg !270, !tbaa !271
  %84 = add i64 %83, %62, !dbg !272
  store i64 %84, i64* %82, align 8, !dbg !273, !tbaa !271
  br label %85, !dbg !274

; <label>:85:                                     ; preds = %16, %31, %35, %38, %80, %1
  %86 = phi i32 [ 0, %1 ], [ 2, %80 ], [ 2, %38 ], [ 2, %35 ], [ 2, %31 ], [ 2, %16 ]
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %13) #3, !dbg !275
  ret i32 %86, !dbg !275
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #2

; Function Attrs: nounwind readnone speculatable
declare i16 @llvm.bswap.i16(i16) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #2

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!81, !82, !83}
!llvm.ident = !{!84}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "xdp_stats_map", scope: !2, file: !3, line: 23, type: !73, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !42, globals: !47)
!3 = !DIFile(filename: "xdp_prog_kern.c", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!4 = !{!5, !13}
!5 = !DICompositeType(tag: DW_TAG_enumeration_type, name: "xdp_action", file: !6, line: 876, size: 32, elements: !7)
!6 = !DIFile(filename: "/usr/include/linux/bpf.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!7 = !{!8, !9, !10, !11, !12}
!8 = !DIEnumerator(name: "XDP_ABORTED", value: 0)
!9 = !DIEnumerator(name: "XDP_DROP", value: 1)
!10 = !DIEnumerator(name: "XDP_PASS", value: 2)
!11 = !DIEnumerator(name: "XDP_TX", value: 3)
!12 = !DIEnumerator(name: "XDP_REDIRECT", value: 4)
!13 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !14, line: 28, size: 32, elements: !15)
!14 = !DIFile(filename: "/usr/include/linux/in.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!15 = !{!16, !17, !18, !19, !20, !21, !22, !23, !24, !25, !26, !27, !28, !29, !30, !31, !32, !33, !34, !35, !36, !37, !38, !39, !40, !41}
!16 = !DIEnumerator(name: "IPPROTO_IP", value: 0)
!17 = !DIEnumerator(name: "IPPROTO_ICMP", value: 1)
!18 = !DIEnumerator(name: "IPPROTO_IGMP", value: 2)
!19 = !DIEnumerator(name: "IPPROTO_IPIP", value: 4)
!20 = !DIEnumerator(name: "IPPROTO_TCP", value: 6)
!21 = !DIEnumerator(name: "IPPROTO_EGP", value: 8)
!22 = !DIEnumerator(name: "IPPROTO_PUP", value: 12)
!23 = !DIEnumerator(name: "IPPROTO_UDP", value: 17)
!24 = !DIEnumerator(name: "IPPROTO_IDP", value: 22)
!25 = !DIEnumerator(name: "IPPROTO_TP", value: 29)
!26 = !DIEnumerator(name: "IPPROTO_DCCP", value: 33)
!27 = !DIEnumerator(name: "IPPROTO_IPV6", value: 41)
!28 = !DIEnumerator(name: "IPPROTO_RSVP", value: 46)
!29 = !DIEnumerator(name: "IPPROTO_GRE", value: 47)
!30 = !DIEnumerator(name: "IPPROTO_ESP", value: 50)
!31 = !DIEnumerator(name: "IPPROTO_AH", value: 51)
!32 = !DIEnumerator(name: "IPPROTO_MTP", value: 92)
!33 = !DIEnumerator(name: "IPPROTO_BEETPH", value: 94)
!34 = !DIEnumerator(name: "IPPROTO_ENCAP", value: 98)
!35 = !DIEnumerator(name: "IPPROTO_PIM", value: 103)
!36 = !DIEnumerator(name: "IPPROTO_COMP", value: 108)
!37 = !DIEnumerator(name: "IPPROTO_SCTP", value: 132)
!38 = !DIEnumerator(name: "IPPROTO_UDPLITE", value: 136)
!39 = !DIEnumerator(name: "IPPROTO_MPLS", value: 137)
!40 = !DIEnumerator(name: "IPPROTO_RAW", value: 255)
!41 = !DIEnumerator(name: "IPPROTO_MAX", value: 256)
!42 = !{!43, !44, !45}
!43 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!44 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!45 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !46, size: 64)
!46 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!47 = !{!0, !48, !54, !62}
!48 = !DIGlobalVariableExpression(var: !49, expr: !DIExpression())
!49 = distinct !DIGlobalVariable(name: "_license", scope: !2, file: !3, line: 108, type: !50, isLocal: false, isDefinition: true)
!50 = !DICompositeType(tag: DW_TAG_array_type, baseType: !51, size: 32, elements: !52)
!51 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!52 = !{!53}
!53 = !DISubrange(count: 4)
!54 = !DIGlobalVariableExpression(var: !55, expr: !DIExpression())
!55 = distinct !DIGlobalVariable(name: "bpf_map_lookup_elem", scope: !2, file: !56, line: 33, type: !57, isLocal: true, isDefinition: true)
!56 = !DIFile(filename: "../dependencies/libbpf/src//build/usr/include/bpf/bpf_helper_defs.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!57 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !58, size: 64)
!58 = !DISubroutineType(types: !59)
!59 = !{!43, !43, !60}
!60 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !61, size: 64)
!61 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!62 = !DIGlobalVariableExpression(var: !63, expr: !DIExpression())
!63 = distinct !DIGlobalVariable(name: "bpf_trace_printk", scope: !2, file: !56, line: 152, type: !64, isLocal: true, isDefinition: true)
!64 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !65, size: 64)
!65 = !DISubroutineType(types: !66)
!66 = !{!67, !68, !70, null}
!67 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!68 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !69, size: 64)
!69 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !51)
!70 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u32", file: !71, line: 27, baseType: !72)
!71 = !DIFile(filename: "/usr/include/asm-generic/int-ll64.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!72 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!73 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_map_def", file: !74, line: 33, size: 160, elements: !75)
!74 = !DIFile(filename: "../dependencies/libbpf/src//build/usr/include/bpf/bpf_helpers.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!75 = !{!76, !77, !78, !79, !80}
!76 = !DIDerivedType(tag: DW_TAG_member, name: "type", scope: !73, file: !74, line: 34, baseType: !72, size: 32)
!77 = !DIDerivedType(tag: DW_TAG_member, name: "key_size", scope: !73, file: !74, line: 35, baseType: !72, size: 32, offset: 32)
!78 = !DIDerivedType(tag: DW_TAG_member, name: "value_size", scope: !73, file: !74, line: 36, baseType: !72, size: 32, offset: 64)
!79 = !DIDerivedType(tag: DW_TAG_member, name: "max_entries", scope: !73, file: !74, line: 37, baseType: !72, size: 32, offset: 96)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "map_flags", scope: !73, file: !74, line: 38, baseType: !72, size: 32, offset: 128)
!81 = !{i32 2, !"Dwarf Version", i32 4}
!82 = !{i32 2, !"Debug Info Version", i32 3}
!83 = !{i32 1, !"wchar_size", i32 4}
!84 = !{!"clang version 6.0.1-svn334776-1~exp1~20190309042730.123 (branches/release_60)"}
!85 = distinct !DISubprogram(name: "xdp_stats1_func", scope: !3, file: !3, line: 38, type: !86, isLocal: false, isDefinition: true, scopeLine: 39, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !94)
!86 = !DISubroutineType(types: !87)
!87 = !{!67, !88}
!88 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !89, size: 64)
!89 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xdp_md", file: !6, line: 887, size: 96, elements: !90)
!90 = !{!91, !92, !93}
!91 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !89, file: !6, line: 888, baseType: !70, size: 32)
!92 = !DIDerivedType(tag: DW_TAG_member, name: "data_end", scope: !89, file: !6, line: 889, baseType: !70, size: 32, offset: 32)
!93 = !DIDerivedType(tag: DW_TAG_member, name: "data_meta", scope: !89, file: !6, line: 890, baseType: !70, size: 32, offset: 64)
!94 = !{!95, !96, !97, !98, !99, !110, !111, !112, !127, !128, !129, !148, !170, !175, !176}
!95 = !DILocalVariable(name: "ctx", arg: 1, scope: !85, file: !3, line: 38, type: !88)
!96 = !DILocalVariable(name: "data_end", scope: !85, file: !3, line: 41, type: !43)
!97 = !DILocalVariable(name: "data", scope: !85, file: !3, line: 42, type: !43)
!98 = !DILocalVariable(name: "action", scope: !85, file: !3, line: 46, type: !70)
!99 = !DILocalVariable(name: "rec", scope: !85, file: !3, line: 48, type: !100)
!100 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !101, size: 64)
!101 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "datarec", file: !102, line: 8, size: 256, elements: !103)
!102 = !DIFile(filename: "./common_kern_user.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!103 = !{!104, !107, !108, !109}
!104 = !DIDerivedType(tag: DW_TAG_member, name: "rx_packets", scope: !101, file: !102, line: 9, baseType: !105, size: 64)
!105 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u64", file: !71, line: 31, baseType: !106)
!106 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!107 = !DIDerivedType(tag: DW_TAG_member, name: "rx_bytes", scope: !101, file: !102, line: 10, baseType: !105, size: 64, offset: 64)
!108 = !DIDerivedType(tag: DW_TAG_member, name: "rx_tcp_open", scope: !101, file: !102, line: 11, baseType: !105, size: 64, offset: 128)
!109 = !DIDerivedType(tag: DW_TAG_member, name: "rx_tcp_payload", scope: !101, file: !102, line: 12, baseType: !105, size: 64, offset: 192)
!110 = !DILocalVariable(name: "bytes", scope: !85, file: !3, line: 53, type: !105)
!111 = !DILocalVariable(name: "payload_size", scope: !85, file: !3, line: 59, type: !72)
!112 = !DILocalVariable(name: "eth", scope: !85, file: !3, line: 60, type: !113)
!113 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !114, size: 64)
!114 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "ethhdr", file: !115, line: 159, size: 112, elements: !116)
!115 = !DIFile(filename: "/usr/include/linux/if_ether.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!116 = !{!117, !121, !122}
!117 = !DIDerivedType(tag: DW_TAG_member, name: "h_dest", scope: !114, file: !115, line: 160, baseType: !118, size: 48)
!118 = !DICompositeType(tag: DW_TAG_array_type, baseType: !46, size: 48, elements: !119)
!119 = !{!120}
!120 = !DISubrange(count: 6)
!121 = !DIDerivedType(tag: DW_TAG_member, name: "h_source", scope: !114, file: !115, line: 161, baseType: !118, size: 48, offset: 48)
!122 = !DIDerivedType(tag: DW_TAG_member, name: "h_proto", scope: !114, file: !115, line: 162, baseType: !123, size: 16, offset: 96)
!123 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be16", file: !124, line: 25, baseType: !125)
!124 = !DIFile(filename: "/usr/include/linux/types.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!125 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u16", file: !71, line: 24, baseType: !126)
!126 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!127 = !DILocalVariable(name: "payload", scope: !85, file: !3, line: 61, type: !45)
!128 = !DILocalVariable(name: "payload_offset", scope: !85, file: !3, line: 62, type: !72)
!129 = !DILocalVariable(name: "ip", scope: !85, file: !3, line: 63, type: !130)
!130 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !131, size: 64)
!131 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "iphdr", file: !132, line: 86, size: 160, elements: !133)
!132 = !DIFile(filename: "/usr/include/linux/ip.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!133 = !{!134, !136, !137, !138, !139, !140, !141, !142, !143, !145, !147}
!134 = !DIDerivedType(tag: DW_TAG_member, name: "ihl", scope: !131, file: !132, line: 88, baseType: !135, size: 4, flags: DIFlagBitField, extraData: i64 0)
!135 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u8", file: !71, line: 21, baseType: !46)
!136 = !DIDerivedType(tag: DW_TAG_member, name: "version", scope: !131, file: !132, line: 89, baseType: !135, size: 4, offset: 4, flags: DIFlagBitField, extraData: i64 0)
!137 = !DIDerivedType(tag: DW_TAG_member, name: "tos", scope: !131, file: !132, line: 96, baseType: !135, size: 8, offset: 8)
!138 = !DIDerivedType(tag: DW_TAG_member, name: "tot_len", scope: !131, file: !132, line: 97, baseType: !123, size: 16, offset: 16)
!139 = !DIDerivedType(tag: DW_TAG_member, name: "id", scope: !131, file: !132, line: 98, baseType: !123, size: 16, offset: 32)
!140 = !DIDerivedType(tag: DW_TAG_member, name: "frag_off", scope: !131, file: !132, line: 99, baseType: !123, size: 16, offset: 48)
!141 = !DIDerivedType(tag: DW_TAG_member, name: "ttl", scope: !131, file: !132, line: 100, baseType: !135, size: 8, offset: 64)
!142 = !DIDerivedType(tag: DW_TAG_member, name: "protocol", scope: !131, file: !132, line: 101, baseType: !135, size: 8, offset: 72)
!143 = !DIDerivedType(tag: DW_TAG_member, name: "check", scope: !131, file: !132, line: 102, baseType: !144, size: 16, offset: 80)
!144 = !DIDerivedType(tag: DW_TAG_typedef, name: "__sum16", file: !124, line: 31, baseType: !125)
!145 = !DIDerivedType(tag: DW_TAG_member, name: "saddr", scope: !131, file: !132, line: 103, baseType: !146, size: 32, offset: 96)
!146 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be32", file: !124, line: 27, baseType: !70)
!147 = !DIDerivedType(tag: DW_TAG_member, name: "daddr", scope: !131, file: !132, line: 104, baseType: !146, size: 32, offset: 128)
!148 = !DILocalVariable(name: "tcp", scope: !85, file: !3, line: 64, type: !149)
!149 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !150, size: 64)
!150 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "tcphdr", file: !151, line: 25, size: 160, elements: !152)
!151 = !DIFile(filename: "/usr/include/linux/tcp.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/XDP-Code/http-counter")
!152 = !{!153, !154, !155, !156, !157, !158, !159, !160, !161, !162, !163, !164, !165, !166, !167, !168, !169}
!153 = !DIDerivedType(tag: DW_TAG_member, name: "source", scope: !150, file: !151, line: 26, baseType: !123, size: 16)
!154 = !DIDerivedType(tag: DW_TAG_member, name: "dest", scope: !150, file: !151, line: 27, baseType: !123, size: 16, offset: 16)
!155 = !DIDerivedType(tag: DW_TAG_member, name: "seq", scope: !150, file: !151, line: 28, baseType: !146, size: 32, offset: 32)
!156 = !DIDerivedType(tag: DW_TAG_member, name: "ack_seq", scope: !150, file: !151, line: 29, baseType: !146, size: 32, offset: 64)
!157 = !DIDerivedType(tag: DW_TAG_member, name: "res1", scope: !150, file: !151, line: 31, baseType: !125, size: 4, offset: 96, flags: DIFlagBitField, extraData: i64 96)
!158 = !DIDerivedType(tag: DW_TAG_member, name: "doff", scope: !150, file: !151, line: 32, baseType: !125, size: 4, offset: 100, flags: DIFlagBitField, extraData: i64 96)
!159 = !DIDerivedType(tag: DW_TAG_member, name: "fin", scope: !150, file: !151, line: 33, baseType: !125, size: 1, offset: 104, flags: DIFlagBitField, extraData: i64 96)
!160 = !DIDerivedType(tag: DW_TAG_member, name: "syn", scope: !150, file: !151, line: 34, baseType: !125, size: 1, offset: 105, flags: DIFlagBitField, extraData: i64 96)
!161 = !DIDerivedType(tag: DW_TAG_member, name: "rst", scope: !150, file: !151, line: 35, baseType: !125, size: 1, offset: 106, flags: DIFlagBitField, extraData: i64 96)
!162 = !DIDerivedType(tag: DW_TAG_member, name: "psh", scope: !150, file: !151, line: 36, baseType: !125, size: 1, offset: 107, flags: DIFlagBitField, extraData: i64 96)
!163 = !DIDerivedType(tag: DW_TAG_member, name: "ack", scope: !150, file: !151, line: 37, baseType: !125, size: 1, offset: 108, flags: DIFlagBitField, extraData: i64 96)
!164 = !DIDerivedType(tag: DW_TAG_member, name: "urg", scope: !150, file: !151, line: 38, baseType: !125, size: 1, offset: 109, flags: DIFlagBitField, extraData: i64 96)
!165 = !DIDerivedType(tag: DW_TAG_member, name: "ece", scope: !150, file: !151, line: 39, baseType: !125, size: 1, offset: 110, flags: DIFlagBitField, extraData: i64 96)
!166 = !DIDerivedType(tag: DW_TAG_member, name: "cwr", scope: !150, file: !151, line: 40, baseType: !125, size: 1, offset: 111, flags: DIFlagBitField, extraData: i64 96)
!167 = !DIDerivedType(tag: DW_TAG_member, name: "window", scope: !150, file: !151, line: 55, baseType: !123, size: 16, offset: 112)
!168 = !DIDerivedType(tag: DW_TAG_member, name: "check", scope: !150, file: !151, line: 56, baseType: !144, size: 16, offset: 128)
!169 = !DIDerivedType(tag: DW_TAG_member, name: "urg_ptr", scope: !150, file: !151, line: 57, baseType: !123, size: 16, offset: 144)
!170 = !DILocalVariable(name: "____fmt", scope: !171, file: !3, line: 85, type: !172)
!171 = distinct !DILexicalBlock(scope: !85, file: !3, line: 85, column: 5)
!172 = !DICompositeType(tag: DW_TAG_array_type, baseType: !51, size: 248, elements: !173)
!173 = !{!174}
!174 = !DISubrange(count: 31)
!175 = !DILocalVariable(name: "byte", scope: !85, file: !3, line: 92, type: !45)
!176 = !DILocalVariable(name: "____fmt", scope: !177, file: !3, line: 100, type: !180)
!177 = distinct !DILexicalBlock(scope: !178, file: !3, line: 100, column: 3)
!178 = distinct !DILexicalBlock(scope: !179, file: !3, line: 98, column: 47)
!179 = distinct !DILexicalBlock(scope: !85, file: !3, line: 98, column: 5)
!180 = !DICompositeType(tag: DW_TAG_array_type, baseType: !51, size: 184, elements: !181)
!181 = !{!182}
!182 = !DISubrange(count: 23)
!183 = !DILocation(line: 38, column: 37, scope: !85)
!184 = !DILocation(line: 41, column: 38, scope: !85)
!185 = !{!186, !187, i64 4}
!186 = !{!"xdp_md", !187, i64 0, !187, i64 4, !187, i64 8}
!187 = !{!"int", !188, i64 0}
!188 = !{!"omnipotent char", !189, i64 0}
!189 = !{!"Simple C/C++ TBAA"}
!190 = !DILocation(line: 41, column: 27, scope: !85)
!191 = !DILocation(line: 41, column: 19, scope: !85)
!192 = !DILocation(line: 41, column: 8, scope: !85)
!193 = !DILocation(line: 42, column: 38, scope: !85)
!194 = !{!186, !187, i64 0}
!195 = !DILocation(line: 42, column: 27, scope: !85)
!196 = !DILocation(line: 42, column: 19, scope: !85)
!197 = !DILocation(line: 42, column: 8, scope: !85)
!198 = !DILocation(line: 46, column: 2, scope: !85)
!199 = !DILocation(line: 46, column: 8, scope: !85)
!200 = !{!187, !187, i64 0}
!201 = !DILocation(line: 48, column: 24, scope: !85)
!202 = !DILocation(line: 48, column: 18, scope: !85)
!203 = !DILocation(line: 49, column: 7, scope: !204)
!204 = distinct !DILexicalBlock(scope: !85, file: !3, line: 49, column: 6)
!205 = !DILocation(line: 49, column: 6, scope: !85)
!206 = !DILocation(line: 53, column: 25, scope: !85)
!207 = !DILocation(line: 53, column: 8, scope: !85)
!208 = !DILocation(line: 55, column: 7, scope: !85)
!209 = !DILocation(line: 55, column: 17, scope: !85)
!210 = !{!211, !212, i64 0}
!211 = !{!"datarec", !212, i64 0, !212, i64 8, !212, i64 16, !212, i64 24}
!212 = !{!"long long", !188, i64 0}
!213 = !DILocation(line: 56, column: 7, scope: !85)
!214 = !DILocation(line: 56, column: 16, scope: !85)
!215 = !{!211, !212, i64 8}
!216 = !DILocation(line: 66, column: 8, scope: !85)
!217 = !DILocation(line: 60, column: 20, scope: !85)
!218 = !DILocation(line: 68, column: 18, scope: !219)
!219 = distinct !DILexicalBlock(scope: !85, file: !3, line: 68, column: 6)
!220 = !DILocation(line: 68, column: 33, scope: !219)
!221 = !DILocation(line: 72, column: 20, scope: !222)
!222 = distinct !DILexicalBlock(scope: !85, file: !3, line: 72, column: 9)
!223 = !DILocation(line: 72, column: 34, scope: !222)
!224 = !DILocation(line: 68, column: 6, scope: !85)
!225 = !DILocation(line: 63, column: 19, scope: !85)
!226 = !DILocation(line: 75, column: 13, scope: !227)
!227 = distinct !DILexicalBlock(scope: !85, file: !3, line: 75, column: 9)
!228 = !{!229, !188, i64 9}
!229 = !{!"iphdr", !188, i64 0, !188, i64 0, !188, i64 1, !230, i64 2, !230, i64 4, !230, i64 6, !188, i64 8, !188, i64 9, !230, i64 10, !187, i64 12, !187, i64 16}
!230 = !{!"short", !188, i64 0}
!231 = !DILocation(line: 75, column: 22, scope: !227)
!232 = !DILocation(line: 75, column: 9, scope: !85)
!233 = !DILocation(line: 79, column: 21, scope: !234)
!234 = distinct !DILexicalBlock(scope: !85, file: !3, line: 79, column: 9)
!235 = !DILocation(line: 79, column: 36, scope: !234)
!236 = !DILocation(line: 79, column: 9, scope: !85)
!237 = !DILocation(line: 83, column: 20, scope: !85)
!238 = !{!229, !230, i64 2}
!239 = !DILocation(line: 83, column: 51, scope: !85)
!240 = !DILocation(line: 83, column: 55, scope: !85)
!241 = !DILocation(line: 83, column: 43, scope: !85)
!242 = !DILocation(line: 83, column: 66, scope: !85)
!243 = !DILocation(line: 83, column: 69, scope: !85)
!244 = !DILocation(line: 83, column: 59, scope: !85)
!245 = !DILocation(line: 59, column: 15, scope: !85)
!246 = !DILocation(line: 85, column: 5, scope: !171)
!247 = !{!248, !230, i64 12}
!248 = !{!"ethhdr", !188, i64 0, !188, i64 6, !230, i64 12}
!249 = !DILocation(line: 85, column: 5, scope: !85)
!250 = !DILocation(line: 61, column: 20, scope: !85)
!251 = !DILocation(line: 89, column: 25, scope: !252)
!252 = distinct !DILexicalBlock(scope: !85, file: !3, line: 89, column: 9)
!253 = !DILocation(line: 89, column: 40, scope: !252)
!254 = !DILocation(line: 93, column: 21, scope: !255)
!255 = distinct !DILexicalBlock(scope: !85, file: !3, line: 93, column: 8)
!256 = !DILocation(line: 93, column: 25, scope: !255)
!257 = !DILocation(line: 89, column: 9, scope: !85)
!258 = !DILocation(line: 92, column: 20, scope: !85)
!259 = !DILocation(line: 92, column: 48, scope: !85)
!260 = !DILocation(line: 98, column: 6, scope: !179)
!261 = !{!188, !188, i64 0}
!262 = !DILocation(line: 98, column: 24, scope: !179)
!263 = !DILocation(line: 99, column: 27, scope: !178)
!264 = !{!211, !212, i64 16}
!265 = !DILocation(line: 99, column: 39, scope: !178)
!266 = !DILocation(line: 99, column: 20, scope: !178)
!267 = !DILocation(line: 100, column: 3, scope: !177)
!268 = !DILocation(line: 100, column: 3, scope: !178)
!269 = !DILocation(line: 101, column: 5, scope: !178)
!270 = !DILocation(line: 103, column: 32, scope: !85)
!271 = !{!211, !212, i64 24}
!272 = !DILocation(line: 103, column: 47, scope: !85)
!273 = !DILocation(line: 103, column: 25, scope: !85)
!274 = !DILocation(line: 105, column: 2, scope: !85)
!275 = !DILocation(line: 106, column: 1, scope: !85)
