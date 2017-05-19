# HACApK_BEM-BB_FPGA_1.1.1

parallelc ブランチについて

C言語版カーネルが並列実行に対応できていなかったので対応させた。
その際に全体の挙動を見直して修正を加えたため、その内容を記しておく。

- 全体的な挙動（プログラムの流れ）について
m_HACApK_use.f90 / HACApK_gensolv を見るとわかるように、
元プログラムではHACApK_solveで一度ソルバーを実行した上でHACApK_measurez～関数にて
FortranとCそれぞれのMat-Vecベンチマークを実行し、
さらにC版Mat-Vecを含むHACApK_solve_caxを実行するという（妙な？）流れになっていた。
HACApK_gensolv関数を変更し、以下のような流れにした。
if(Mat-Vecベンチマークをしたいだけの場合){
  hacapk_solve : 必要なのかよくわからないが残した
  hacapk_measurez_time_ax_lfmtx : FortranによるMat-Vec
  hacapk_setupc : FortranとCの連結のための処理を分離したもの
  hacapk_measurez_time_ax_FPGA_lfmtx : CによるMat-Vec
}else{
  if(C版Mat-Vecカーネルを使うbicgstabを使いたい場合){
    hacapk_setupc : FortranとCの連結のための処理を分離したもの
	HACApK_solve_cax : C版Mat-Vecカーネルを使うbicgstabを含む
  }else{
	HACApK_solve : 内部でbicgstabとgcrmに分岐
  }
}

カーネルの選択にはst_ctl%param(85)を流用。
-1: C版Mat-Vecカーネルを使うbicgstab（新設）
0: Mat-Vecカーネルベンチマークのみ（新設）
1: BiCGSTAB
2: GCM(m)

実行時に使うものの指定にはbem-bb-config.txtの第2パラメタを利用。
BICGSTAB_C を指定するとC版のBiCGSTAB、
MATVEC を指定するとMat-Vecカーネルのベンチマークのみを実行（ソルバーを起動しない）。

