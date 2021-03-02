; ModuleID = 'xdp_prog_kern.c'
source_filename = "xdp_prog_kern.c"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "bpf"

%struct.bpf_map_def = type { i32, i32, i32, i32, i32 }
%struct.xdp_md = type { i32, i32, i32, i32, i32 }
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
  call void @llvm.dbg.value(metadata %struct.xdp_md* %0, metadata !97, metadata !DIExpression()), !dbg !185
  %5 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 1, !dbg !186
  %6 = load i32, i32* %5, align 4, !dbg !186, !tbaa !187
  %7 = zext i32 %6 to i64, !dbg !192
  %8 = inttoptr i64 %7 to i8*, !dbg !193
  call void @llvm.dbg.value(metadata i8* %8, metadata !98, metadata !DIExpression()), !dbg !194
  %9 = getelementptr inbounds %struct.xdp_md, %struct.xdp_md* %0, i64 0, i32 0, !dbg !195
  %10 = load i32, i32* %9, align 4, !dbg !195, !tbaa !196
  %11 = zext i32 %10 to i64, !dbg !197
  %12 = inttoptr i64 %11 to i8*, !dbg !198
  call void @llvm.dbg.value(metadata i8* %12, metadata !99, metadata !DIExpression()), !dbg !199
  %13 = bitcast i32* %2 to i8*, !dbg !200
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %13) #3, !dbg !200
  call void @llvm.dbg.value(metadata i32 0, metadata !100, metadata !DIExpression()), !dbg !201
  store i32 0, i32* %2, align 4, !dbg !201, !tbaa !202
  %14 = call i8* inttoptr (i64 1 to i8* (i8*, i8*)*)(i8* bitcast (%struct.bpf_map_def* @xdp_stats_map to i8*), i8* nonnull %13) #3, !dbg !203
  call void @llvm.dbg.value(metadata i8* %14, metadata !101, metadata !DIExpression()), !dbg !204
  %15 = icmp eq i8* %14, null, !dbg !205
  br i1 %15, label %85, label %16, !dbg !207

; <label>:16:                                     ; preds = %1
  %17 = sub nsw i64 %7, %11, !dbg !208
  call void @llvm.dbg.value(metadata i64 %17, metadata !112, metadata !DIExpression()), !dbg !209
  %18 = bitcast i8* %14 to i64*, !dbg !210
  %19 = load i64, i64* %18, align 8, !dbg !211, !tbaa !212
  %20 = add i64 %19, 1, !dbg !211
  store i64 %20, i64* %18, align 8, !dbg !211, !tbaa !212
  %21 = getelementptr inbounds i8, i8* %14, i64 8, !dbg !215
  %22 = bitcast i8* %21 to i64*, !dbg !215
  %23 = load i64, i64* %22, align 8, !dbg !216, !tbaa !217
  %24 = add i64 %17, %23, !dbg !216
  store i64 %24, i64* %22, align 8, !dbg !216, !tbaa !217
  %25 = inttoptr i64 %11 to %struct.ethhdr*, !dbg !218
  call void @llvm.dbg.value(metadata %struct.ethhdr* %25, metadata !114, metadata !DIExpression()), !dbg !219
  %26 = getelementptr i8, i8* %12, i64 14, !dbg !220
  %27 = icmp ugt i8* %26, %8, !dbg !222
  %28 = getelementptr i8, i8* %12, i64 34, !dbg !223
  %29 = icmp ugt i8* %28, %8, !dbg !225
  %30 = or i1 %27, %29, !dbg !226
  call void @llvm.dbg.value(metadata i8* %26, metadata !131, metadata !DIExpression()), !dbg !227
  br i1 %30, label %85, label %31, !dbg !226

; <label>:31:                                     ; preds = %16
  %32 = getelementptr inbounds i8, i8* %12, i64 23, !dbg !228
  %33 = load i8, i8* %32, align 1, !dbg !228, !tbaa !230
  %34 = icmp eq i8 %33, 6, !dbg !233
  br i1 %34, label %35, label %85, !dbg !234

; <label>:35:                                     ; preds = %31
  %36 = getelementptr i8, i8* %12, i64 54, !dbg !235
  %37 = icmp ugt i8* %36, %8, !dbg !237
  br i1 %37, label %85, label %38, !dbg !238

; <label>:38:                                     ; preds = %35
  %39 = getelementptr inbounds i8, i8* %12, i64 16, !dbg !239
  %40 = bitcast i8* %39 to i16*, !dbg !239
  %41 = load i16, i16* %40, align 2, !dbg !239, !tbaa !240
  %42 = call i16 @llvm.bswap.i16(i16 %41), !dbg !239
  %43 = zext i16 %42 to i32, !dbg !239
  %44 = getelementptr inbounds i8, i8* %12, i64 46, !dbg !241
  %45 = bitcast i8* %44 to i16*, !dbg !241
  %46 = load i16, i16* %45, align 4, !dbg !241
  %47 = lshr i16 %46, 2, !dbg !242
  %48 = and i16 %47, 60, !dbg !242
  %49 = zext i16 %48 to i32, !dbg !242
  %50 = sub nsw i32 %43, %49, !dbg !243
  %51 = load i8, i8* %26, align 4, !dbg !244
  %52 = shl i8 %51, 2, !dbg !245
  %53 = and i8 %52, 60, !dbg !245
  %54 = zext i8 %53 to i32, !dbg !245
  %55 = sub nsw i32 %50, %54, !dbg !246
  call void @llvm.dbg.value(metadata i32 %55, metadata !113, metadata !DIExpression()), !dbg !247
  %56 = getelementptr inbounds [31 x i8], [31 x i8]* %3, i64 0, i64 0, !dbg !248
  call void @llvm.lifetime.start.p0i8(i64 31, i8* nonnull %56) #3, !dbg !248
  call void @llvm.dbg.declare(metadata [31 x i8]* %3, metadata !172, metadata !DIExpression()), !dbg !248
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %56, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @xdp_stats1_func.____fmt, i64 0, i64 0), i64 31, i32 1, i1 false), !dbg !248
  %57 = getelementptr inbounds %struct.ethhdr, %struct.ethhdr* %25, i64 0, i32 2, !dbg !248
  %58 = load i16, i16* %57, align 1, !dbg !248, !tbaa !249
  %59 = call i16 @llvm.bswap.i16(i16 %58), !dbg !248
  %60 = zext i16 %59 to i32, !dbg !248
  %61 = call i32 (i8*, i32, ...) inttoptr (i64 6 to i32 (i8*, i32, ...)*)(i8* nonnull %56, i32 31, i32 %55, i32 %60) #3, !dbg !248
  call void @llvm.lifetime.end.p0i8(i64 31, i8* nonnull %56) #3, !dbg !251
  call void @llvm.dbg.value(metadata i8* %36, metadata !129, metadata !DIExpression()), !dbg !252
  %62 = zext i32 %55 to i64, !dbg !253
  %63 = getelementptr i8, i8* %36, i64 %62, !dbg !253
  %64 = icmp ugt i8* %63, %8, !dbg !255
  %65 = getelementptr i8, i8* %12, i64 48, !dbg !256
  %66 = icmp ugt i8* %65, %8, !dbg !258
  %67 = or i1 %66, %64, !dbg !259
  call void @llvm.dbg.value(metadata i8* %69, metadata !177, metadata !DIExpression()), !dbg !260
  br i1 %67, label %85, label %68, !dbg !259

; <label>:68:                                     ; preds = %38
  %69 = getelementptr inbounds i8, i8* %12, i64 47, !dbg !261
  %70 = load i8, i8* %69, align 1, !dbg !262, !tbaa !263
  %71 = and i8 %70, 18, !dbg !264
  %72 = icmp eq i8 %71, 18, !dbg !264
  br i1 %72, label %73, label %80, !dbg !264

; <label>:73:                                     ; preds = %68
  %74 = getelementptr inbounds i8, i8* %14, i64 16, !dbg !265
  %75 = bitcast i8* %74 to i64*, !dbg !265
  %76 = load i64, i64* %75, align 8, !dbg !265, !tbaa !266
  %77 = add i64 %76, 1, !dbg !267
  store i64 %77, i64* %75, align 8, !dbg !268, !tbaa !266
  %78 = getelementptr inbounds [23 x i8], [23 x i8]* %4, i64 0, i64 0, !dbg !269
  call void @llvm.lifetime.start.p0i8(i64 23, i8* nonnull %78) #3, !dbg !269
  call void @llvm.dbg.declare(metadata [23 x i8]* %4, metadata !178, metadata !DIExpression()), !dbg !269
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull %78, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @xdp_stats1_func.____fmt.1, i64 0, i64 0), i64 23, i32 1, i1 false), !dbg !269
  %79 = call i32 (i8*, i32, ...) inttoptr (i64 6 to i32 (i8*, i32, ...)*)(i8* nonnull %78, i32 23) #3, !dbg !269
  call void @llvm.lifetime.end.p0i8(i64 23, i8* nonnull %78) #3, !dbg !270
  br label %80, !dbg !271

; <label>:80:                                     ; preds = %68, %73
  %81 = getelementptr inbounds i8, i8* %14, i64 24, !dbg !272
  %82 = bitcast i8* %81 to i64*, !dbg !272
  %83 = load i64, i64* %82, align 8, !dbg !272, !tbaa !273
  %84 = add i64 %83, %62, !dbg !274
  store i64 %84, i64* %82, align 8, !dbg !275, !tbaa !273
  br label %85, !dbg !276

; <label>:85:                                     ; preds = %16, %31, %35, %38, %80, %1
  %86 = phi i32 [ 0, %1 ], [ 2, %80 ], [ 2, %38 ], [ 2, %35 ], [ 2, %31 ], [ 2, %16 ]
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %13) #3, !dbg !277
  ret i32 %86, !dbg !277
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
!3 = !DIFile(filename: "xdp_prog_kern.c", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!4 = !{!5, !13}
!5 = !DICompositeType(tag: DW_TAG_enumeration_type, name: "xdp_action", file: !6, line: 2845, size: 32, elements: !7)
!6 = !DIFile(filename: "../headers/linux/bpf.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!7 = !{!8, !9, !10, !11, !12}
!8 = !DIEnumerator(name: "XDP_ABORTED", value: 0)
!9 = !DIEnumerator(name: "XDP_DROP", value: 1)
!10 = !DIEnumerator(name: "XDP_PASS", value: 2)
!11 = !DIEnumerator(name: "XDP_TX", value: 3)
!12 = !DIEnumerator(name: "XDP_REDIRECT", value: 4)
!13 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !14, line: 28, size: 32, elements: !15)
!14 = !DIFile(filename: "/usr/include/linux/in.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
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
!56 = !DIFile(filename: "../libbpf/src//build/usr/include/bpf/bpf_helper_defs.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
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
!71 = !DIFile(filename: "/usr/include/asm-generic/int-ll64.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!72 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!73 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "bpf_map_def", file: !74, line: 33, size: 160, elements: !75)
!74 = !DIFile(filename: "../libbpf/src//build/usr/include/bpf/bpf_helpers.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
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
!85 = distinct !DISubprogram(name: "xdp_stats1_func", scope: !3, file: !3, line: 38, type: !86, isLocal: false, isDefinition: true, scopeLine: 39, flags: DIFlagPrototyped, isOptimized: true, unit: !2, variables: !96)
!86 = !DISubroutineType(types: !87)
!87 = !{!67, !88}
!88 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !89, size: 64)
!89 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "xdp_md", file: !6, line: 2856, size: 160, elements: !90)
!90 = !{!91, !92, !93, !94, !95}
!91 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !89, file: !6, line: 2857, baseType: !70, size: 32)
!92 = !DIDerivedType(tag: DW_TAG_member, name: "data_end", scope: !89, file: !6, line: 2858, baseType: !70, size: 32, offset: 32)
!93 = !DIDerivedType(tag: DW_TAG_member, name: "data_meta", scope: !89, file: !6, line: 2859, baseType: !70, size: 32, offset: 64)
!94 = !DIDerivedType(tag: DW_TAG_member, name: "ingress_ifindex", scope: !89, file: !6, line: 2861, baseType: !70, size: 32, offset: 96)
!95 = !DIDerivedType(tag: DW_TAG_member, name: "rx_queue_index", scope: !89, file: !6, line: 2862, baseType: !70, size: 32, offset: 128)
!96 = !{!97, !98, !99, !100, !101, !112, !113, !114, !129, !130, !131, !150, !172, !177, !178}
!97 = !DILocalVariable(name: "ctx", arg: 1, scope: !85, file: !3, line: 38, type: !88)
!98 = !DILocalVariable(name: "data_end", scope: !85, file: !3, line: 41, type: !43)
!99 = !DILocalVariable(name: "data", scope: !85, file: !3, line: 42, type: !43)
!100 = !DILocalVariable(name: "action", scope: !85, file: !3, line: 46, type: !70)
!101 = !DILocalVariable(name: "rec", scope: !85, file: !3, line: 48, type: !102)
!102 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !103, size: 64)
!103 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "datarec", file: !104, line: 8, size: 256, elements: !105)
!104 = !DIFile(filename: "./common_kern_user.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!105 = !{!106, !109, !110, !111}
!106 = !DIDerivedType(tag: DW_TAG_member, name: "rx_packets", scope: !103, file: !104, line: 9, baseType: !107, size: 64)
!107 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u64", file: !71, line: 31, baseType: !108)
!108 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!109 = !DIDerivedType(tag: DW_TAG_member, name: "rx_bytes", scope: !103, file: !104, line: 10, baseType: !107, size: 64, offset: 64)
!110 = !DIDerivedType(tag: DW_TAG_member, name: "rx_tcp_open", scope: !103, file: !104, line: 11, baseType: !107, size: 64, offset: 128)
!111 = !DIDerivedType(tag: DW_TAG_member, name: "rx_tcp_payload", scope: !103, file: !104, line: 12, baseType: !107, size: 64, offset: 192)
!112 = !DILocalVariable(name: "bytes", scope: !85, file: !3, line: 53, type: !107)
!113 = !DILocalVariable(name: "payload_size", scope: !85, file: !3, line: 59, type: !72)
!114 = !DILocalVariable(name: "eth", scope: !85, file: !3, line: 60, type: !115)
!115 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !116, size: 64)
!116 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "ethhdr", file: !117, line: 159, size: 112, elements: !118)
!117 = !DIFile(filename: "/usr/include/linux/if_ether.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!118 = !{!119, !123, !124}
!119 = !DIDerivedType(tag: DW_TAG_member, name: "h_dest", scope: !116, file: !117, line: 160, baseType: !120, size: 48)
!120 = !DICompositeType(tag: DW_TAG_array_type, baseType: !46, size: 48, elements: !121)
!121 = !{!122}
!122 = !DISubrange(count: 6)
!123 = !DIDerivedType(tag: DW_TAG_member, name: "h_source", scope: !116, file: !117, line: 161, baseType: !120, size: 48, offset: 48)
!124 = !DIDerivedType(tag: DW_TAG_member, name: "h_proto", scope: !116, file: !117, line: 162, baseType: !125, size: 16, offset: 96)
!125 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be16", file: !126, line: 25, baseType: !127)
!126 = !DIFile(filename: "/usr/include/linux/types.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!127 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u16", file: !71, line: 24, baseType: !128)
!128 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!129 = !DILocalVariable(name: "payload", scope: !85, file: !3, line: 61, type: !45)
!130 = !DILocalVariable(name: "payload_offset", scope: !85, file: !3, line: 62, type: !72)
!131 = !DILocalVariable(name: "ip", scope: !85, file: !3, line: 63, type: !132)
!132 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !133, size: 64)
!133 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "iphdr", file: !134, line: 86, size: 160, elements: !135)
!134 = !DIFile(filename: "/usr/include/linux/ip.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!135 = !{!136, !138, !139, !140, !141, !142, !143, !144, !145, !147, !149}
!136 = !DIDerivedType(tag: DW_TAG_member, name: "ihl", scope: !133, file: !134, line: 88, baseType: !137, size: 4, flags: DIFlagBitField, extraData: i64 0)
!137 = !DIDerivedType(tag: DW_TAG_typedef, name: "__u8", file: !71, line: 21, baseType: !46)
!138 = !DIDerivedType(tag: DW_TAG_member, name: "version", scope: !133, file: !134, line: 89, baseType: !137, size: 4, offset: 4, flags: DIFlagBitField, extraData: i64 0)
!139 = !DIDerivedType(tag: DW_TAG_member, name: "tos", scope: !133, file: !134, line: 96, baseType: !137, size: 8, offset: 8)
!140 = !DIDerivedType(tag: DW_TAG_member, name: "tot_len", scope: !133, file: !134, line: 97, baseType: !125, size: 16, offset: 16)
!141 = !DIDerivedType(tag: DW_TAG_member, name: "id", scope: !133, file: !134, line: 98, baseType: !125, size: 16, offset: 32)
!142 = !DIDerivedType(tag: DW_TAG_member, name: "frag_off", scope: !133, file: !134, line: 99, baseType: !125, size: 16, offset: 48)
!143 = !DIDerivedType(tag: DW_TAG_member, name: "ttl", scope: !133, file: !134, line: 100, baseType: !137, size: 8, offset: 64)
!144 = !DIDerivedType(tag: DW_TAG_member, name: "protocol", scope: !133, file: !134, line: 101, baseType: !137, size: 8, offset: 72)
!145 = !DIDerivedType(tag: DW_TAG_member, name: "check", scope: !133, file: !134, line: 102, baseType: !146, size: 16, offset: 80)
!146 = !DIDerivedType(tag: DW_TAG_typedef, name: "__sum16", file: !126, line: 31, baseType: !127)
!147 = !DIDerivedType(tag: DW_TAG_member, name: "saddr", scope: !133, file: !134, line: 103, baseType: !148, size: 32, offset: 96)
!148 = !DIDerivedType(tag: DW_TAG_typedef, name: "__be32", file: !126, line: 27, baseType: !70)
!149 = !DIDerivedType(tag: DW_TAG_member, name: "daddr", scope: !133, file: !134, line: 104, baseType: !148, size: 32, offset: 128)
!150 = !DILocalVariable(name: "tcp", scope: !85, file: !3, line: 64, type: !151)
!151 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !152, size: 64)
!152 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "tcphdr", file: !153, line: 25, size: 160, elements: !154)
!153 = !DIFile(filename: "/usr/include/linux/tcp.h", directory: "/home/francois/Documents/ULG/TravailFinEtude/github/TFE/map-counter")
!154 = !{!155, !156, !157, !158, !159, !160, !161, !162, !163, !164, !165, !166, !167, !168, !169, !170, !171}
!155 = !DIDerivedType(tag: DW_TAG_member, name: "source", scope: !152, file: !153, line: 26, baseType: !125, size: 16)
!156 = !DIDerivedType(tag: DW_TAG_member, name: "dest", scope: !152, file: !153, line: 27, baseType: !125, size: 16, offset: 16)
!157 = !DIDerivedType(tag: DW_TAG_member, name: "seq", scope: !152, file: !153, line: 28, baseType: !148, size: 32, offset: 32)
!158 = !DIDerivedType(tag: DW_TAG_member, name: "ack_seq", scope: !152, file: !153, line: 29, baseType: !148, size: 32, offset: 64)
!159 = !DIDerivedType(tag: DW_TAG_member, name: "res1", scope: !152, file: !153, line: 31, baseType: !127, size: 4, offset: 96, flags: DIFlagBitField, extraData: i64 96)
!160 = !DIDerivedType(tag: DW_TAG_member, name: "doff", scope: !152, file: !153, line: 32, baseType: !127, size: 4, offset: 100, flags: DIFlagBitField, extraData: i64 96)
!161 = !DIDerivedType(tag: DW_TAG_member, name: "fin", scope: !152, file: !153, line: 33, baseType: !127, size: 1, offset: 104, flags: DIFlagBitField, extraData: i64 96)
!162 = !DIDerivedType(tag: DW_TAG_member, name: "syn", scope: !152, file: !153, line: 34, baseType: !127, size: 1, offset: 105, flags: DIFlagBitField, extraData: i64 96)
!163 = !DIDerivedType(tag: DW_TAG_member, name: "rst", scope: !152, file: !153, line: 35, baseType: !127, size: 1, offset: 106, flags: DIFlagBitField, extraData: i64 96)
!164 = !DIDerivedType(tag: DW_TAG_member, name: "psh", scope: !152, file: !153, line: 36, baseType: !127, size: 1, offset: 107, flags: DIFlagBitField, extraData: i64 96)
!165 = !DIDerivedType(tag: DW_TAG_member, name: "ack", scope: !152, file: !153, line: 37, baseType: !127, size: 1, offset: 108, flags: DIFlagBitField, extraData: i64 96)
!166 = !DIDerivedType(tag: DW_TAG_member, name: "urg", scope: !152, file: !153, line: 38, baseType: !127, size: 1, offset: 109, flags: DIFlagBitField, extraData: i64 96)
!167 = !DIDerivedType(tag: DW_TAG_member, name: "ece", scope: !152, file: !153, line: 39, baseType: !127, size: 1, offset: 110, flags: DIFlagBitField, extraData: i64 96)
!168 = !DIDerivedType(tag: DW_TAG_member, name: "cwr", scope: !152, file: !153, line: 40, baseType: !127, size: 1, offset: 111, flags: DIFlagBitField, extraData: i64 96)
!169 = !DIDerivedType(tag: DW_TAG_member, name: "window", scope: !152, file: !153, line: 55, baseType: !125, size: 16, offset: 112)
!170 = !DIDerivedType(tag: DW_TAG_member, name: "check", scope: !152, file: !153, line: 56, baseType: !146, size: 16, offset: 128)
!171 = !DIDerivedType(tag: DW_TAG_member, name: "urg_ptr", scope: !152, file: !153, line: 57, baseType: !125, size: 16, offset: 144)
!172 = !DILocalVariable(name: "____fmt", scope: !173, file: !3, line: 85, type: !174)
!173 = distinct !DILexicalBlock(scope: !85, file: !3, line: 85, column: 5)
!174 = !DICompositeType(tag: DW_TAG_array_type, baseType: !51, size: 248, elements: !175)
!175 = !{!176}
!176 = !DISubrange(count: 31)
!177 = !DILocalVariable(name: "byte", scope: !85, file: !3, line: 92, type: !45)
!178 = !DILocalVariable(name: "____fmt", scope: !179, file: !3, line: 100, type: !182)
!179 = distinct !DILexicalBlock(scope: !180, file: !3, line: 100, column: 3)
!180 = distinct !DILexicalBlock(scope: !181, file: !3, line: 98, column: 47)
!181 = distinct !DILexicalBlock(scope: !85, file: !3, line: 98, column: 5)
!182 = !DICompositeType(tag: DW_TAG_array_type, baseType: !51, size: 184, elements: !183)
!183 = !{!184}
!184 = !DISubrange(count: 23)
!185 = !DILocation(line: 38, column: 37, scope: !85)
!186 = !DILocation(line: 41, column: 38, scope: !85)
!187 = !{!188, !189, i64 4}
!188 = !{!"xdp_md", !189, i64 0, !189, i64 4, !189, i64 8, !189, i64 12, !189, i64 16}
!189 = !{!"int", !190, i64 0}
!190 = !{!"omnipotent char", !191, i64 0}
!191 = !{!"Simple C/C++ TBAA"}
!192 = !DILocation(line: 41, column: 27, scope: !85)
!193 = !DILocation(line: 41, column: 19, scope: !85)
!194 = !DILocation(line: 41, column: 8, scope: !85)
!195 = !DILocation(line: 42, column: 38, scope: !85)
!196 = !{!188, !189, i64 0}
!197 = !DILocation(line: 42, column: 27, scope: !85)
!198 = !DILocation(line: 42, column: 19, scope: !85)
!199 = !DILocation(line: 42, column: 8, scope: !85)
!200 = !DILocation(line: 46, column: 2, scope: !85)
!201 = !DILocation(line: 46, column: 8, scope: !85)
!202 = !{!189, !189, i64 0}
!203 = !DILocation(line: 48, column: 24, scope: !85)
!204 = !DILocation(line: 48, column: 18, scope: !85)
!205 = !DILocation(line: 49, column: 7, scope: !206)
!206 = distinct !DILexicalBlock(scope: !85, file: !3, line: 49, column: 6)
!207 = !DILocation(line: 49, column: 6, scope: !85)
!208 = !DILocation(line: 53, column: 25, scope: !85)
!209 = !DILocation(line: 53, column: 8, scope: !85)
!210 = !DILocation(line: 55, column: 7, scope: !85)
!211 = !DILocation(line: 55, column: 17, scope: !85)
!212 = !{!213, !214, i64 0}
!213 = !{!"datarec", !214, i64 0, !214, i64 8, !214, i64 16, !214, i64 24}
!214 = !{!"long long", !190, i64 0}
!215 = !DILocation(line: 56, column: 7, scope: !85)
!216 = !DILocation(line: 56, column: 16, scope: !85)
!217 = !{!213, !214, i64 8}
!218 = !DILocation(line: 66, column: 8, scope: !85)
!219 = !DILocation(line: 60, column: 20, scope: !85)
!220 = !DILocation(line: 68, column: 18, scope: !221)
!221 = distinct !DILexicalBlock(scope: !85, file: !3, line: 68, column: 6)
!222 = !DILocation(line: 68, column: 33, scope: !221)
!223 = !DILocation(line: 72, column: 20, scope: !224)
!224 = distinct !DILexicalBlock(scope: !85, file: !3, line: 72, column: 9)
!225 = !DILocation(line: 72, column: 34, scope: !224)
!226 = !DILocation(line: 68, column: 6, scope: !85)
!227 = !DILocation(line: 63, column: 19, scope: !85)
!228 = !DILocation(line: 75, column: 13, scope: !229)
!229 = distinct !DILexicalBlock(scope: !85, file: !3, line: 75, column: 9)
!230 = !{!231, !190, i64 9}
!231 = !{!"iphdr", !190, i64 0, !190, i64 0, !190, i64 1, !232, i64 2, !232, i64 4, !232, i64 6, !190, i64 8, !190, i64 9, !232, i64 10, !189, i64 12, !189, i64 16}
!232 = !{!"short", !190, i64 0}
!233 = !DILocation(line: 75, column: 22, scope: !229)
!234 = !DILocation(line: 75, column: 9, scope: !85)
!235 = !DILocation(line: 79, column: 21, scope: !236)
!236 = distinct !DILexicalBlock(scope: !85, file: !3, line: 79, column: 9)
!237 = !DILocation(line: 79, column: 36, scope: !236)
!238 = !DILocation(line: 79, column: 9, scope: !85)
!239 = !DILocation(line: 83, column: 20, scope: !85)
!240 = !{!231, !232, i64 2}
!241 = !DILocation(line: 83, column: 51, scope: !85)
!242 = !DILocation(line: 83, column: 55, scope: !85)
!243 = !DILocation(line: 83, column: 43, scope: !85)
!244 = !DILocation(line: 83, column: 66, scope: !85)
!245 = !DILocation(line: 83, column: 69, scope: !85)
!246 = !DILocation(line: 83, column: 59, scope: !85)
!247 = !DILocation(line: 59, column: 15, scope: !85)
!248 = !DILocation(line: 85, column: 5, scope: !173)
!249 = !{!250, !232, i64 12}
!250 = !{!"ethhdr", !190, i64 0, !190, i64 6, !232, i64 12}
!251 = !DILocation(line: 85, column: 5, scope: !85)
!252 = !DILocation(line: 61, column: 20, scope: !85)
!253 = !DILocation(line: 89, column: 25, scope: !254)
!254 = distinct !DILexicalBlock(scope: !85, file: !3, line: 89, column: 9)
!255 = !DILocation(line: 89, column: 40, scope: !254)
!256 = !DILocation(line: 93, column: 21, scope: !257)
!257 = distinct !DILexicalBlock(scope: !85, file: !3, line: 93, column: 8)
!258 = !DILocation(line: 93, column: 25, scope: !257)
!259 = !DILocation(line: 89, column: 9, scope: !85)
!260 = !DILocation(line: 92, column: 20, scope: !85)
!261 = !DILocation(line: 92, column: 48, scope: !85)
!262 = !DILocation(line: 98, column: 6, scope: !181)
!263 = !{!190, !190, i64 0}
!264 = !DILocation(line: 98, column: 24, scope: !181)
!265 = !DILocation(line: 99, column: 27, scope: !180)
!266 = !{!213, !214, i64 16}
!267 = !DILocation(line: 99, column: 39, scope: !180)
!268 = !DILocation(line: 99, column: 20, scope: !180)
!269 = !DILocation(line: 100, column: 3, scope: !179)
!270 = !DILocation(line: 100, column: 3, scope: !180)
!271 = !DILocation(line: 101, column: 5, scope: !180)
!272 = !DILocation(line: 103, column: 32, scope: !85)
!273 = !{!213, !214, i64 24}
!274 = !DILocation(line: 103, column: 47, scope: !85)
!275 = !DILocation(line: 103, column: 25, scope: !85)
!276 = !DILocation(line: 105, column: 2, scope: !85)
!277 = !DILocation(line: 106, column: 1, scope: !85)
