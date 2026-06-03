import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.patches import FancyBboxPatch

T_SEQ    = 1.43
threads  = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
t_par    = [1.4950, 1.0738, 0.6429, 0.6151, 0.5054,
            0.4546, 0.4227, 0.4518, 0.4588, 0.4175, 0.4052, 0.4713]
speedup  = [round(T_SEQ / t, 2) for t in t_par]
efic     = [round(s / t, 2) for s, t in zip(speedup, threads)]
ideal    = threads

# ── layout: tabela à esquerda, dois gráficos à direita ──────────────────────
fig = plt.figure(figsize=(15, 6))
fig.patch.set_facecolor('#f8f9fa')
gs  = gridspec.GridSpec(2, 2, width_ratios=[1, 1.6], hspace=0.45, wspace=0.35,
                        left=0.04, right=0.97, top=0.88, bottom=0.10)

ax_tab = fig.add_subplot(gs[:, 0])   # tabela ocupa as duas linhas
ax_t   = fig.add_subplot(gs[0, 1])   # gráfico tempo (linha de cima)
ax_s   = fig.add_subplot(gs[1, 1])   # gráfico speedup (linha de baixo)

fig.suptitle("P1 — Multiplicação de Matrizes  1200×1200\n"
             "Intel Core i7-1355U  ·  Ubuntu 22.04  ·  GCC 11.4  ·  -O2 -pthread",
             fontsize=12, fontweight='bold', color='#1a1a2e')

# ── tabela ───────────────────────────────────────────────────────────────────
ax_tab.axis('off')

col_labels = ["Threads", "T_par (s)", "Speedup", "Eficiência"]
rows = [[str(t), f"{tp:.4f}", f"{s:.2f}×", f"{e:.2f}"]
        for t, tp, s, e in zip(threads, t_par, speedup, efic)]

# cores alternadas por linha; destaca melhor speedup
best_idx = speedup.index(max(speedup))
row_colors = []
for i in range(len(rows)):
    if i == best_idx:
        row_colors.append(['#d4edda'] * 4)   # verde suave — melhor speedup
    elif i % 2 == 0:
        row_colors.append(['#ffffff'] * 4)
    else:
        row_colors.append(['#eef2f7'] * 4)

tbl = ax_tab.table(
    cellText=rows,
    colLabels=col_labels,
    cellLoc='center',
    loc='center',
    cellColours=row_colors,
)
tbl.auto_set_font_size(False)
tbl.set_fontsize(10)
tbl.scale(1.18, 1.55)

# cabeçalho escuro
for j in range(len(col_labels)):
    tbl[(0, j)].set_facecolor('#1a1a2e')
    tbl[(0, j)].set_text_props(color='white', fontweight='bold')

# borda suave em todas as células
for key, cell in tbl.get_celld().items():
    cell.set_edgecolor('#cccccc')
    cell.set_linewidth(0.5)

ax_tab.set_title("Escalabilidade — varredura 1 a 12 threads",
                 fontsize=10, pad=8, color='#1a1a2e', fontweight='bold')

# nota de rodapé da tabela
ax_tab.text(0.5, 0.01, f"T_seq de referência = {T_SEQ} s  ·  linha verde = melhor speedup",
            ha='center', va='bottom', fontsize=7.5, color='#555555',
            transform=ax_tab.transAxes)

# ── gráfico 1: tempo ─────────────────────────────────────────────────────────
ax_t.plot(threads, t_par, marker='o', color='#2176ae', linewidth=2,
          markersize=5, label='T_par medido')
ax_t.axhline(y=T_SEQ, color='#888888', linestyle='--', linewidth=1.2,
             label=f'T_seq = {T_SEQ} s')
ax_t.set_facecolor('#fdfdfd')
ax_t.set_xlabel("Threads", fontsize=9)
ax_t.set_ylabel("Tempo (s)", fontsize=9)
ax_t.set_title("Tempo de execução × threads", fontsize=10, fontweight='bold', color='#1a1a2e')
ax_t.set_xticks(threads)
ax_t.tick_params(labelsize=8)
ax_t.legend(fontsize=8)
ax_t.grid(True, linestyle=':', alpha=0.5)

# ── gráfico 2: speedup ───────────────────────────────────────────────────────
ax_s.plot(threads, speedup, marker='o', color='#2176ae', linewidth=2,
          markersize=5, label='Speedup real')
ax_s.plot(threads, ideal, linestyle='--', color='#e05c5c', linewidth=1.4,
          label='Speedup ideal')
ax_s.set_facecolor('#fdfdfd')
ax_s.set_xlabel("Threads", fontsize=9)
ax_s.set_ylabel("Speedup", fontsize=9)
ax_s.set_title("Speedup × threads", fontsize=10, fontweight='bold', color='#1a1a2e')
ax_s.set_xticks(threads)
ax_s.tick_params(labelsize=8)
ax_s.legend(fontsize=8)
ax_s.grid(True, linestyle=':', alpha=0.5)

plt.savefig("speedup.png", dpi=160, bbox_inches='tight', facecolor=fig.get_facecolor())
print("Gráfico salvo em speedup.png")
